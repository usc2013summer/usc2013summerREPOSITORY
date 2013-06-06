#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "visitor.h"
#include "driver.h"

#define TICKETTAKER_MAX 10
#define VISITOR_MAX 10
#define CAR_MAX 10
#define LIMO_MAX 10

extern void visitorThread(int);
extern void driverThread(int);

List* limoWaitings = new List;
List* carWaitings = new List;
Lock* ticketTakerLocks[TICKETTAKER_MAX];

void Problem2(){
	Thread* t;
	Lock* carLock;
	Condition* carCV;
	
	int carNum, visitorNum;
	int carID = 0, visitorID = 0, driverID = 0;
	int visitorTotal = 3, carTotal = 4;
	int ticketTakerTotal = 4;
	
	for(carNum=0 ; carNum<carTotal ; carNum++){
		carLock = new Lock("LOCK");
		carCV = new Condition("CONDITION");
		
		Driver* driver = new Driver(driverID++);
		driver->setDriver(driver);
		driver->setCarID(carID++);
		driver->setVisitorNum(visitorTotal);
		driver->setLockCV(carLock, carCV);
		
		for(visitorNum=0 ; visitorNum<visitorTotal ; visitorNum++){
			Visitor* visitor = new Visitor(visitorID++);
			visitor->setVisitor(visitor);
			visitor->setLockCV(carLock, carCV);
			visitor->setMyDriver(driver);
				
			t = new Thread("THREAD");
			t->Fork(visitorThread, (int)visitor);
		}
		
		if( rand()%10 < 5 )
			driver->setCarModel("Limousine");
		else
			driver->setCarModel("Car");
		
		t = new Thread("THREAD");
		t->Fork(driverThread, (int)driver);
	}
}	