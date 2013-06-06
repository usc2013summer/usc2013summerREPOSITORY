#include "visitor.h"
#include <iostream>
#include <string>

Visitor::Visitor(int newVisitorID){
	visitorID = newVisitorID;
}

void Visitor::setVisitor(Visitor* newVisitor){
	visitor = newVisitor;
}

void Visitor::setLockCV(Lock* newCarLock, Condition* newCarCV){
	carLock = newCarLock;
	carCV = newCarCV;
}

void Visitor::setMyDriver(Driver* newDriver){
	myDriver = newDriver;
}

void Visitor::stayInCar(){
	carLock->Acquire();
	carCV->Wait(carLock);
	carLock->Release();
}

void Visitor::getOutFromCar(){
	printf("Visitor %d has been told to exit Car %d\n", visitorID, myDriver->getCarID());
	myDriver->oneVisitorOutFromCar();
	printf("Visitor %d has exited Car %d\n", visitorID, myDriver->getCarID());
}

void Visitor::goToTicketTaker(){
	printf("Visitor %d is waiting for Ticket Taker %d\n", visitorID, myTicketTakerID);
}

void visitorThread(int newVisitor){
	Visitor* visitor = (Visitor*)newVisitor;
	
	visitor->stayInCar();
	visitor->getOutFromCar();
	visitor->goToTicketTaker();
}
