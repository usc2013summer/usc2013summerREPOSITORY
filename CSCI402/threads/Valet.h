/*
 *  Valet.h
 *  
 *
 *  Created by Elsen Wiraatmadja on 6/6/13.
 *  Copyright 2013 University of Southern California. All rights reserved.
 *
 */

#ifndef VALET_H
#define DRIVER_H
#include synch.h
#include shared.h

class Valet{
	private:
	Valet* valet;
	int valetID;
	
	public:
	Valet(int);
	void setValet(Valet*);
}