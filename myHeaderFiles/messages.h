#ifndef MESSAGES_H
#define MESSAGES_H

#define MSGLENGTH 3

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
	}
}

void rstMsgTracker(){
	tracker = 0;
	m1Full = false;
}

void addCharToMsg(unsigned char var){
	switch(tracker){
		case 0: m1.validity = var; break;
		case 1: m1.address = var; break;
		case 2: m1.cmd = var; break; 
	}
	incMsgTracker();
} 

#endif