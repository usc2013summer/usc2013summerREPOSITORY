#ifndef DRIVER_H
#define DRIVER_H

#include "synch.h"

class Driver{
	private:
		Driver* driver;
		int driverID;
		int carID;
		int visitorNum;
		int count;
		char* carModel;
		Lock* carLock;
		Condition* carCV;
		bool visitorsOutOfCar;
		
	public:
		// Constructor, Setter, Getter
		Driver(int);
		void setDriver(Driver*);
		void setLockCV(Lock*, Condition*);
		void setLineLock(Lock*);
		void setVisitorNum(int);
		void setCarModel(char*);
		void setCarID(int);
		void setValetQ(List*);
		int getCarID();
		
		
		// Actions
		void arrived();
		void tellVisitorsGetOut();
		
		// By Other Threads
		void oneVisitorOutFromCar();
};

#endif
