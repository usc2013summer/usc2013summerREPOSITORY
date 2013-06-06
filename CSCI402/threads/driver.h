#ifndef DRIVER_H
#define DRIVER_H

#include "synch.h"

class Driver{
	private:
		int driverID;
		int carID;
		int visitorNum;
		int count;
		int myValet;
		int myToken;
		char* carModel;
		Lock* carLock;
		Condition* carCV;
		bool visitorsOutOfCar;
		
	public:
		// Constructor, Setter, Getter
		Driver(int);
		void setMyValet(int);
		void setMyToken(int);
		void setLockCV(Lock*, Condition*);
		void setLineLock(Lock*);
		void setVisitorNum(int);
		void setCarModel(char*);
		void setCarID(int);
		void setValetQ(List*);
		int getCarID();
		char* getCarModel();
		int getDriverID();
		
		// Actions
		void arrived();
		void tellVisitorsGetOut();
		void playMuseum();
		
		// By Other Threads
		void oneVisitorOutFromCar();
};

#endif
