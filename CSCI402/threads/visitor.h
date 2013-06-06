#ifndef VISITOR_H
#define VISITOR_H

#include "synch.h"
#include "driver.h"

class Visitor{
	private:
		Visitor* visitor;
		int visitorID;
		int myTicketTakerID;
		Lock* carLock;
		Condition* carCV;
		Driver* myDriver;
		
	public:
		// Constructor, Setter, Getter
		Visitor(int);
		void setVisitor(Visitor*);
		void setLockCV(Lock*, Condition*);
		void setMyDriver(Driver*);
		void setMyTicketTakerID(int);
		
		// Action
		void stayInCar();
		void getOutFromCar();
		void goToTicketTaker();
};

#endif
