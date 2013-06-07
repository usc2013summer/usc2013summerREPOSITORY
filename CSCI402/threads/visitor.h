#ifndef VISITOR_H
#define VISITOR_H

#include "synch.h"
#include "driver.h"

class Visitor{
	private:
		int visitorID;
		int groupID;
		int myTicketTakerID;
		Lock* carLock;
		Condition* carCV;
		Driver* myDriver;
		
	public:
		// Constructor, Setter, Getter
		Visitor(int);
		void setLockCV(Lock*, Condition*);
		void setMyDriver(Driver*);
		void setMyTicketTakerID(int);
		void setVisitorID(int);
		void setGroupID(int);
		int getVisitorID();
		int getGroupID(int);
		
		// Action
		void stayInCar();
		void getOutFromCar();
		void goToTicketTaker();
};

#endif
