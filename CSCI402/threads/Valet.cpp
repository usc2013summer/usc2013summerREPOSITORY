/*
 *  Valet.cpp
 *  
 *
 *  Created by Elsen Wiraatmadja on 6/6/13.
 *  Copyright 2013 Green River Community College. All rights reserved.
 *
 */

#include "Valet.h"
#include "copyright.h"
#include "system.h"
#include "synch.h"


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

Valet::Valet(int myID){
	valetId = myID;
}

void Valet::setValet(Valet* newValet){
	valet = newValet;
}

void valetThread(int newValet){
	Valet* valet = (Valet*)newValet;
	int myIndex = valet->valetId;
	
	
	while(true){
		
		lineLock->Acquire();
		//If nobody is on the line to park or to pick up cars
		//sleep on the bench until somebody arrives
		if(limoQ->isEmpty() && carQ->isEmpty() && leavingQ->isEmpty()){
			valetQ->Append(v);
			onBench++;
			/*
			 //If there are 2 people on the bench, ask one to go sleep on the back room
			 if(onBench==2){
			 restLock->Acquire();
			 restCV->Signal(restLock);
			 
			 //Wait until getting called by manager
			 backRoomLock->Acquire();
			 restLock->Release();
			 backRoomCV->Wait(backRoomLock);
			 
			 //Got called, need to go to work
			 backRoomLock->Release();
			 }
			 else*/
			lineCV[myIndex]-> Wait(lineLock);
			onBench--;
		}
		
		//Somebody has arrived
		if(!(limoQ->isEmpty()) || !(carQ->isEmpty())){
			
			//Priority goes to limo
			//vehicleIndex needed in Valet class to store the car
			totalVehicle--;
			if (!limoQ->isEmpty()) {
				vehicleIndex = (int)limoQ->Remove();
			}
			else{
				vehicleIndex = (int)carQ->Remove();
			}
			
			//Wake up people inside the car
			vehicleLock[vehicleIndex]->Acquire();
			lineLock->Release();
			vehicleCV->Signal(vehicleLock[vehicleIndex]);
			
			//Wait for the driver to give the vehicle key
			keyLock[vehicleIndex]->Acquire();
			vehicleLock[vehicleIndex]->Release();
			keyCV[vehicleIndex]->Wait(keyLock[vehicleIndex]);
			
			//Give the token to the driver
			tokenLock[vehicleIndex]->Acquire();
			parkingToken++;
			keyLock[vehicleIndex]->Release();
			tokenCV[vehicleIndex]->Signal(tokenLock[vehicleIndex]);
			
			//Wait until driver reads the value
			readingLock[vehicleIndex]->Acquire();
			tokenLock[vehicleIndex])->Release();
			readingCV[vehicleIndex]->Wait(readingLock[vehicleIndex]);
			
			
			//Park the vehicle
			readingLock[vehicleIndex])->Release();
			int r = rand() % 16 + 5;
			for(int i=0; i<rand; i++)
				currentThread->Yield();
			
		}
		//Somebody wants to pick up their car
		else {
			vehicleIndex = (int)leavingQ->Remove();
			
			//Acquire token from the driver
			tokenLock[vehicleIndex]->Acquire();
			lineLock->Release();
			tokenCV->Signal(tokenLock[vehicleIndex]);
			
			//Taking the vehicle back, wait for tip
			tipLock[vehicleIndex]->Acquire();
			int r = rand() % 16 + 5;
			for(int i=0; i<rand; i++)
				currentThread->Yield();
			tokenLock[vehicleIndex]->Release();
			tipCV[vehicleIndex]->Wait(tipLock[vehicleIndex]);
			
			//Got the tip already
			tipLock[vehicleIndex]->Release();
		}
	}
}
	



