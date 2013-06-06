#include "driver.h"
#include "copyright.h"
#include "system.h"

extern int totalVehicle;
extern Lock* lineLock;
extern List* limoQ, carQ;
extern List* valetQ;	
extern Lock* keyLock[];
extern Condition* keyCV[];
extern Lock* vehicleLock[];
extern Condition* vehicleCV[];
extern Lock* tokenLock[];
extern Condition* tokenCV[];
extern Lock* readingLock[];
extern Condition* readingCV[];
extern Lock* tipLock[];
extern Condition* tipCV[];
extern List* leavingQ;

Driver::Driver(int newDriverID){
	driverID = newDriverID;
	visitorsOutOfCar = false;
	count = 0;
}

void Driver::setMyValet(int newMyValet){
	myValet = newMyValet;
}

void Driver::setMyToken(int newMyToken){
	myToken = newMyToken;
}

void Driver::setLockCV(Lock* newCarLock, Condition* newCarCV){
	carLock = newCarLock;
	carCV = newCarCV;
}

void Driver::setVisitorNum(int newVisitorNum){
	visitorNum = newVisitorNum;
}

void Driver::setCarModel(char* newCarModel){
	carModel = newCarModel;
}

void Driver::setCarID(int newCarID){
	carID = newCarID;
}

int Driver::getCarID(){
	return carID;
}

char* Driver::getCarModel(){
	return carModel;
}

int Driver::getDriverID(){
	return driverID;
}

void Driver::arrived(){
	carLock->Acquire();
	carCV->Broadcast(carLock);
	carLock->Release();
}

void Driver::tellVisitorsGetOut(){
	printf("%s Driver %d has told his visitors to leave Car %d\n", carModel, driverID, carID);
	
	// Driver is still in a car
	while( count < visitorNum ){
		carLock->Acquire();
		carCV->Wait(carLock);
		carLock->Release();
	}
}

void Driver::playMuseum(){
	int random = rand() % 51 + 50;
    for(int i=0; i<random; i++)
		currentThread->Yield();
}

void Driver::oneVisitorOutFromCar(){
	count++;
	carLock->Acquire();
	carCV->Signal(carLock);
	carLock->Release();
}

void driverThread(int newDriver){
	Driver* driver = (Driver*)newDriver;
	int myIndex = driver->getCarID();
	int myValet, myToken;
	
	driver->arrived();
	driver->tellVisitorsGetOut();
	
	///// INTERACTION WITH VALET START /////
	// interaction with valet
	lineLock->Acquire();

    //Put the vehicle on the queue
    if(/*this is a car*/)
        carQ->Append(driver);totalVehicle++;
    else /*this is a limo*/
        limoQ->Append(driver);totalVehicle++;

    //If there's a valet sleeping on the bench, wake him up.
    if(!valetQ->isEmpty()){
        myValet = (int)valetQ->Remove();
        lineCV[myValet]->Signal(lineLock);
    }

    vehicleLock[myIndex]->Acquire();        //Acquire my vehicle lock
    lineLock->Release();
    vehicleCV[myIndex]->Wait(vehicleLock[myIndex]); //Wait until approached by Valet

    //VALET HAS COME TO OUR CAR.
	/**
    I NEED TO WAKE EVERYBODY UP NOW
    **/
    //AFTER EVERYONE HAS EXIT THE CAR

    //Give the key to the Valet
	printf("%s Driver %d has given their keys to Parking Valet %d for Car %d\n", driver->getCarModel(), driver->getDriverID(), myValet, driver->getCarID());
    keyLock[myIndex]->Acquire();
    vehicleLock[myIndex]->Release();
    keyCV->Signal(keyLock[myIndex]);

    //Wait for the valet to give the token
    tokenLock[myIndex]->Acquire();
    keyLock[myIndex]->Release();
    tokenCV[myIndex]->Wait(tokenLock[myIndex]);
	
    readingLock[myIndex]->Acquire();
    myToken = parkingToken;
    tokenLock[myIndex]->Release();
    readingCV[myIndex]->Signal(readingLock[myIndex]);
	printf("%s Driver %d has received Parking Token %d from Parking Valet %d for Car %d\n", driver->getCarModel(), driver->getDriverID(), myToken, myValet, driver->getCarID());
	readingLock[myIndex]->Release();
	///// INTERACTION WITH VALET END /////
	
	///// INTERACTION WITH TICKET TAKER START /////
	
	
	///// INTERACTION WITH TICKET TAKER END /////
	
	
    //Go play in the museum
	if( driver->getCarModel() == "Car" )
		driver->playMuseum();
	else if( driver->getCarModle() == "Limousine" )
		// Hang Around
		
		
	///// INTERACTION WITH VALET START /////
	//AFTER EVERYONE IN MY CAR HAS GOT OUT OF THE MUSEUM
    printf("%s Driver %d has been notified that all their Visitors have left the Museum for Car %d\n", driver->getCarModel(), driver->getDriverID(), driver->getCarID());
	
	//NOW I NEED TO TALK TO THE VALET AND PICK UP MY CAR
    lineLock->Acquire();
    //Put myself on the leaving queue
    leavingQ->Append(d);
    //If there's a valet sleeping on the bench, wake him up.
    if(!valetQ->isEmpty()){
		valetQ->Remove();
        lineCV->Signal(lineLock);
    }
	
	printf("%s Driver %d has given Parking Token %d to Parking Valet %d for Car %d\n", driver->getCarModel(), driver->getDriverID(), myToken, myValet, driver->getCarID());
    //Wait for the valet to get the token from me
    tokenLock[myIndex]->Acquire();
    lineLock->Release();
    tokenCV[myIndex]->Wait(tokenLock[myIndex]);     //Wait until approached by Valet
	
	printf("%s Driver %d has received their keys from Parking Valet %d for Car %d\n", driver->getCarModel(), driver->getDriverID(), myValet, driver->getCarID());
	
    //Take the car and give tip
    tipLock[myIndex]->Acquire();
    tokenLock[myIndex]->Release();
    tipCV[myIndex]->Signal(tipLock[myIndex]);
    tipLock[myIndex]->Release();

	//Go home
	printf("%s Driver %d has left the Museum in Car %d\n", driver->getCarModel(), driver->getDriverID(), driver->getCarID());
	///// INTERACTION WITH VALET END /////
}
