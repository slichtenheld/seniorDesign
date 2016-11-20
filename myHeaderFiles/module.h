/*
 * module.h
 *
 * Created: 11/2/2016 6:04:31 PM
 *  Author: Abel Mak
 */ 


#ifndef MODULE_H_
#define MODULE_H_

// Module Online Status
int M1Online = 0;
int M2Online = 0;
int M3Online = 0;

// Module Dispense Status
int M1Dispense = 0;
int M2Dispense = 0;
int M3Dispense = 0;

// Module Trigger Status
int M1Trigger = 0;
int M2Trigger = 0;
int M3Trigger = 0;

void checkModules() {
	// query each module for 5 secs.
	// MxStatus is set to 1 if module is "online", else set to 0 for "offline"
}

void triggerModule(int module) {
	
}

#endif /* MODULE_H_ */