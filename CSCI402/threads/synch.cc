// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(char* debugName) {
	name = debugName;
	lockOwner = NULL;
	queue = new List;
}

Lock::~Lock() {
	delete queue;
}

void Lock::Acquire() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);	// D.I.
	
	/* I am the lockowner */
	if( lockOwner == currentThread ){ 
		DEBUG( 't', "I am the lockowner" );
	}
	
	/* Lock is available */
	else if( lockOwner == NULL ){
		DEBUG( 't', "Lock is available" );
		lockOwner = currentThread;
	}
	
	/* ELSE */
	else{
		DEBUG( 't', "Acquire : ELSE" );
		queue->Append((void *)currentThread);
		currentThread->Sleep();
	}
	
	(void) interrupt->SetLevel(oldLevel);	// R.I.
}

void Lock::Release() {
	IntStatus oldLevel = interrupt->SetLevel(IntOff);	// D.I.
	
	/* I am NOT lock owner */
	if( lockOwner != currentThread ){
		DEBUG( 't', "I am not lock owner" );
	}
		
	/* A thread is waiting */
	else if( !queue->IsEmpty() ){
		DEBUG( 't', "A thread is waiting" );
		Thread *thread = (Thread *)queue->Remove();
		lockOwner = thread;
		scheduler->ReadyToRun(thread);
	}
	
	/* ELSE */
	else{
		DEBUG( 't', "Release : ELSE" );
		lockOwner = NULL;
	}
	
	(void) interrupt->SetLevel(oldLevel);	// R.I.
}

bool Lock::isHeldByCurrentThread() {
	return lockOwner == currentThread;
}

Condition::Condition(char* debugName) { 
	name = debugName;	
	queue = new List;
	waitingLock = NULL;
}

Condition::~Condition() {
	delete queue;
}

void Condition::Wait(Lock* conditionLock) { 
	IntStatus oldLevel = interrupt->SetLevel(IntOff);	// D.I.
	
	if( conditionLock == NULL ){
		// Application programmer is trying to mess us up.
		// This is not cool.
		printf("ERROR : conditionLock == NULL\n");
		(void) interrupt->SetLevel(oldLevel);	// R.I.
		return;
	}
	
	if( waitingLock == NULL ){
		// This is the first thread calling Wait on this condition variable
		// Associate this lock with this CV
		waitingLock = conditionLock;
	}
	
	if( waitingLock != conditionLock ) {
		// There is a lock associated with this CV.
		// The user did not pass in the same lock already associated.
		printf("ERROR : waitingLock != conditionLock\n");
		(void) interrupt->SetLevel(oldLevel);	// R.I.
		return;
	}
	conditionLock->Release();
	queue->Append((void *)currentThread);
	currentThread->Sleep();
	conditionLock->Acquire();
	
	(void) interrupt->SetLevel(oldLevel);	// R.I.
}

void Condition::Signal(Lock* conditionLock) { 
	IntStatus oldLevel = interrupt->SetLevel(IntOff);	// D.I.
	
	// No Thread is waiting in the wait queue
	if( queue->IsEmpty() ){
		(void) interrupt->SetLevel(oldLevel);	// R.I.
		return;
	}
	
	if( waitingLock != conditionLock ){
		// This is an error. The lock passed by the signaler 
		// must match the lock given up by the waiters.
		// OTherwise, we can have race condition issues.
		printf("ERROR : waitingLock != conditionLock\n");
		(void) interrupt->SetLevel(oldLevel);	// R.I.
		return;
	}
	
	// Everything OK to proceed. Waitup one waiting thread.
	Thread *thread = (Thread *)queue->Remove();
	scheduler->ReadyToRun(thread);
	
	// No more waiting threads
	if( queue->IsEmpty() ){
		waitingLock = NULL;
	}
	
	(void) interrupt->SetLevel(oldLevel);	// R.I.
}

void Condition::Broadcast(Lock* conditionLock) { 
	while( !(queue->IsEmpty()) ){
		Signal(conditionLock);
	}
}
