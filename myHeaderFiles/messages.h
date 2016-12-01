#ifndef MESSAGES_H
#define MESSAGES_H

#define MSGLENGTH 3

#define slaveFrameError '#'
#define slaveDataOverRunError '&'
#define slaveParityError '!'

#define DISPENSE 'a'
#define DISPENSECOMPLETE 'b'
#define STATUS '9'
#define STATUSOK '1'
#define STATUSERROR 0
#define BUSY 'Z'

#define COMMANDINVALID '?'

#define FRAMEERROR -1
#define DATAOVERRUNERROR -2
#define PARITYERROR -3

struct message{
	unsigned char validity;
	unsigned char address;
	unsigned char cmd;
	};
	
uint8_t tracker = 0;
bool m1Full = false;
struct message m1;

void incMsgTracker(){
	tracker++;
	if (tracker >= MSGLENGTH){
		 tracker = 0;
		 m1Full = true;
		 push(m1, &f1); // added this line of code 
	}
}

void rstMsgTracker(){
	tracker = 0;
	m1Full = false;
}

void addCharToMsg(volatile unsigned char var){
	switch(tracker){
		case 0: m1.validity = var; break;
		case 1: m1.address = var; break;
		case 2: m1.cmd = var; break; 
	}
	incMsgTracker();
} 

#endif