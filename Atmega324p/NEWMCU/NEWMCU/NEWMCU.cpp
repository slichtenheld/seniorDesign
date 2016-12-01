/*
 * NEWMCU.cpp
 *
 * Created: 11/30/2016 10:58:51 PM
 *  Author: samuel
 */ 

/*
 * Atmega324p.cpp
 *
 * Created: 10/29/2016 2:35:53 PM
 * Author : samuel
 */ 

#include <stdlib.h> // for rand functionality

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/common.h>
#include <util/delay.h>
#include "C:\Users\samuel\Documents\github\seniorDesign\myHeaderFiles\uart.h"
#include "C:\Users\samuel\Documents\github\seniorDesign\myHeaderFiles\usart.h"
#include "C:\Users\samuel\Documents\github\seniorDesign\myHeaderFiles\timer1.h"
#include "C:\Users\samuel\Documents\github\seniorDesign\myHeaderFiles\messages.h"
#include "C:\Users\samuel\Documents\github\seniorDesign\myHeaderFiles\Fifo.h"


#define OSC_CAL 0x59
//#define BAUD_1 3000                                  
//#define BAUDRATE_1 ((F_CPU)/(BAUD_1*16UL)-1)

#define SIMULATETESTING 1

void clock_calibrate(); 
void restartTransaction();


volatile unsigned char msgCmd; //uart interrupt will put data here
//uint8_t msgID; // value used to keep track of messages
volatile int8_t msgCmdStatus;
volatile int8_t outStandingCmds;
volatile int8_t waitingForResp; // increment when waiting for response
volatile bool timeout0; // flag for timeout0s
volatile bool timeout1;

volatile unsigned char msgResp; // response from module via usart
volatile int8_t msgRespStatus;
volatile int8_t recievedResp; // increment when receive responses

struct fifo f1;


int main(void)
{
	// initialize test LED ports
	//DDRA = 0xFF; // initialize to test
	//DDRC = (1 << DDRC0);
	
	//calibrate clock
	clock_calibrate();
	
	// initialize uart to talk to esp
	UART0_INIT(BAUD_9600, FALSE, 8, NONE, 1, TRUE);
	
	// initialize timer
	TIMER0_init();
	TIMER0_compareInterruptEnable();
	
	// initialize usart
	USART1_init();
	USART1_receiveInterruptEnable();
	// enable interrupts
	sei();
	
	// Global Variables
	outStandingCmds = 0;
	waitingForResp = 0;
	msgRespStatus = 0;
	msgCmd = 0;
	timeout0 = false;
	
	 //
	//	bool isEmpty(fifo *f){
	//	bool isFull(fifo *f){
	//	message getMsg(fifo *f){
	//	int pop(fifo *f){
	//	int push(message m, fifo *f) {
	//	int getHead(fifo *f){
	//	int getTail(fifo *f){
	fifoInit(&f1);
	
	struct message msgTemp;
	msgTemp.validity = '!';
	msgTemp.address = 100;
	msgTemp.cmd = DISPENSE;
	push(msgTemp,&f1);
	msgTemp.validity = '!';
	msgTemp.address = 101;
	msgTemp.cmd = STATUS;
	push(msgTemp,&f1);
	msgTemp.validity = '!';
	msgTemp.address = 102;
	msgTemp.cmd = STATUS;
	push(msgTemp,&f1);
	
	volatile unsigned char temp;
	volatile bool booltemp = false;
	msgTemp = getMsg(&f1); // retrieve earliest msg from FIFO
	temp = msgTemp.address;
	pop(&f1); // remove msg from FIFO
	msgTemp = getMsg(&f1); // retrieve earliest msg from FIFO
	temp = msgTemp.address;
	pop(&f1); // remove msg from FIFO
	msgTemp = getMsg(&f1); // retrieve earliest msg from FIFO
	temp = msgTemp.address;
	booltemp = isEmpty(&f1);
	pop(&f1); // remove msg from FIFO
	msgTemp = getMsg(&f1); // retrieve earliest msg from FIFO
	temp = msgTemp.address;
	booltemp = isEmpty(&f1);
	pop(&f1); // remove msg from FIFO
	booltemp = isEmpty(&f1);
	asm volatile("nop");
	
	
	while(1){
				
		if(timeout0) { // handle timeout0 
			//restartTransaction();
			waitingForResp = 0;
			timeout0 = false; //clear timeout0 flag
			//USART1_receiveDisable();
		}
		
		#if SIMULATETESTING
		{
			//Simulating incoming commands via uart interrupt
			if ( !isFull(&f1)){ // if no outstanding commands and no outstanding responses
				struct message msgTemp;
				msgTemp.validity = '!';
				msgTemp.address = 'X';
				msgTemp.cmd = DISPENSE;
				push(msgTemp,&f1);
				msgTemp.validity = '!';
				msgTemp.address = 'Y';
				msgTemp.cmd = STATUS;
				push(msgTemp,&f1);
				msgTemp.validity = '!';
				msgTemp.address = 'Z';
				msgTemp.cmd = STATUS;
				push(msgTemp,&f1);
			}
			if (!isEmpty(&f1) && !waitingForResp) { // if there is an outstanding command and not waiting on a response
				struct message msgTmp = getMsg(&f1); // retrieve earliest msg from FIFO
				pop(&f1); // remove msg from FIFO
				//volatile unsigned char temp = 0;
				//temp = msgTmp.address;
				//temp = msgTmp.cmd;
				if (msgTmp.validity == '!'){
					USART1_transmit(msgTmp.address, ADDRESS_MSG);
					USART1_transmit(msgTmp.cmd, DATA_MSG);
					TIMER0_enable();// set timer interrupt for how long to wait for response
					waitingForResp=1; // waiting on response
				}
				//outStandingCmds=0; // command sent // REPLACED BY FIFO
			}
		}
		#else 
		{ // USING ESP
			if (!isEmpty(&f1) && !outStandingCmds && !waitingForResp) { // handle message from ESP
				// handle address
				// m1.address Multi Processor Communication needed
				// set up communication to correct slave
				//msgCmd = m1.cmd; 
				outStandingCmds=1;
				rstMsgTracker();
			}
			if (outStandingCmds && !waitingForResp) { // if there is an outstanding command and not waiting on a response
				// pop off message from FIFO
				struct message tmpMsg;
				tmpMsg = getMsg(&f1);
				
				if (tmpMsg.validity=='!'){
					USART1_transmit(tmpMsg.address);
					USART1_transmit(tmpMsg.cmd);
					TIMER0_enable();// set timer interrupt for how long to wait for response
					waitingForResp=1; // waiting on response
					pop(&f1); // pop off message from fifo
				}
				outStandingCmds=0; // command sent
			}
		}
		#endif
		
		
		/* received message from slave logic */
		if (recievedResp) { // handle response
			TIMER0_disable(); 
			waitingForResp = 0;
			if (msgRespStatus < 0){ // local error
				// retransmit command
				restartTransaction();
				clearReceiveBuffer();
				// set other error output bits
			}
			/* COMMENT OUT IF TESTING RANDOM/ALL MSG VALUES!!!!! */
			else if ( msgResp == slaveFrameError || msgResp == slaveDataOverRunError || msgResp == slaveFrameError ){ //slave error
				// retransmit command
				restartTransaction();
				PORTA = ~PORTA;
			}
			else {
				PORTC ^= (1 << DDRC0);
				//PORTA = msgResp; // for now just echoing response to LEDs
			}
			recievedResp=0; //handled response
		}
	}
}

ISR(USART0_RX_vect) {
	unsigned char temp = UDR0;
	if (temp == '!') {
		rstMsgTracker();
		PORTA = ~PORTA;
	}
	addCharToMsg(temp);
}

ISR(USART1_RX_vect) { // interrupt for receive
	recievedResp=1;
	unsigned char temp = 0;
	msgRespStatus = USART1_receive(&temp); // can't pass directly to msgResp because volatile unsigned char (global var)
	msgResp = temp;
}

ISR (TIMER1_COMPA_vect){
	timeout1 = true;
}

ISR (TIMER0_COMPA_vect){
	timeout0 = true;
}

void restartTransaction(){
	//outStandingCmds=1; //should cause last message to be resent
	waitingForResp=0; //no longer waiting for response, restarting transaction
}


void clock_calibrate(void) {
	OSCCAL = OSC_CAL; // CLK calibration
}
