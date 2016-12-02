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


#define OSC_CAL 0x92
//#define BAUD_1 3000                                  
//#define BAUDRATE_1 ((F_CPU)/(BAUD_1*16UL)-1)

#define SIMULATETESTING 0

void clock_calibrate(); 
void restartTransaction();


volatile unsigned char msgCmd; //uart interrupt will put data here
//uint8_t msgID; // value used to keep track of messages
volatile int8_t msgCmdStatus;
//volatile int8_t outStandingCmds;
volatile int8_t waitingForResp; // increment when waiting for response

volatile bool timeout0; // flag for timeout0s
volatile uint8_t timeout0Counter;

volatile bool timeout1;

volatile unsigned char msgResp; // response from module via usart
volatile int8_t msgRespStatus;
volatile int8_t recievedResp; // increment when receive responses

struct fifo f1;
struct message outstandingMsg;

volatile unsigned char debuggingTemp;

int main(void)
{
	
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
	//outStandingCmds = 0;
	waitingForResp = 0;
	msgRespStatus = 0;
	msgCmd = 0;
	timeout0 = false;
	timeout0Counter = 0;
	
	fifoInit(&f1);
	
	while(1){
				
		if(timeout0) { // handle timeout0 
			//restartTransaction();
			timeout0Counter++;
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
				msgTemp.cmd = DISPENSE;
				push(msgTemp,&f1);
				msgTemp.validity = '!';
				msgTemp.address = 'Z';
				msgTemp.cmd = DISPENSE;
				push(msgTemp,&f1);
				_delay_ms(10000);
				
			}
			//if (!isEmpty(&f1) && !waitingForResp) { // if there is an outstanding command and not waiting on a response
				//outstandingMsg = getMsg(&f1); // retrieve earliest msg from FIFO
				//if (timeout0Counter>=3){ // after tried # times, remove from FIFO
					//pop(&f1); // remove msg from FIFO
					//// send response to esp
					//transmitUART0(outstandingMsg.address);
					//transmitUART0(outstandingMsg.cmd);
					//transmitUART0('0'); // not actually zero, verified with Abel
					//transmitUART0('#');
					//timeout0Counter = 0; // also set to zero if received message
				//}
				//else if (outstandingMsg.validity == '!'){
					//USART1_transmit(outstandingMsg.address, ADDRESS_MSG);
					//USART1_transmit(outstandingMsg.cmd, DATA_MSG);
					//TIMER0_enable();// set timer interrupt for how long to wait for response
					//waitingForResp=1; // waiting on response
				//}
				////outStandingCmds=0; // command sent // REPLACED BY FIFO
			//}
		}
		#else 
		{ // USING ESP
			if (m1Full){ // if m1full then put onto FIFO
				push(m1,&f1);
				rstMsgTracker();
				volatile unsigned char temp1 = 0;
				temp1 = m1.validity;
				temp1 = m1.address;
				temp1 = m1.cmd;
				temp1 = 255;
			}
		}
		#endif
			if (!isEmpty(&f1) && !waitingForResp) { // if there is an outstanding command and not waiting on a response
				outstandingMsg = getMsg(&f1); // retrieve earliest msg from FIFO
				
				// **** IF TIMEOUT FROM SLAVE ***** //
				if (timeout0Counter>=30){ // after tried # times, remove from FIFO
					pop(&f1); // remove msg from FIFO
					// send response to esp
					transmitUART0(outstandingMsg.address);
					transmitUART0(outstandingMsg.cmd);
					transmitUART0('0'); // not actually zero, verified with Abel
					transmitUART0('#');
					timeout0Counter = 0; // also set to zero if received message
				}
				
				else {
					
					/******* CHECK MESSAGE WITH DEBUGGER *********/
					debuggingTemp = outstandingMsg.validity;
					debuggingTemp = outstandingMsg.address;
					debuggingTemp = outstandingMsg.cmd;
					
					/*********************************************/
					USART1_transmit(outstandingMsg.address, ADDRESS_MSG);
					USART1_transmit(outstandingMsg.cmd, DATA_MSG);
					TIMER0_enable();// set timer interrupt for how long to wait for response
					waitingForResp = 1; // waiting on response
				}
				//outStandingCmds=0; // command sent // REPLACED BY FIFO
			}
			//if (!isEmpty(&f1) && !outStandingCmds && !waitingForResp) { // handle message from ESP
				//// handle address
				//// m1.address Multi Processor Communication needed
				//// set up communication to correct slave
				////msgCmd = m1.cmd; 
				//outStandingCmds=1;
				//rstMsgTracker();
			//}
			//if (outStandingCmds && !waitingForResp) { // if there is an outstanding command and not waiting on a response
				//// pop off message from FIFO
				//struct message tmpMsg;
				//tmpMsg = getMsg(&f1);
				//
				//if (tmpMsg.validity=='!'){
					//USART1_transmit(tmpMsg.address);
					//USART1_transmit(tmpMsg.cmd);
					//TIMER0_enable();// set timer interrupt for how long to wait for response
					//waitingForResp=1; // waiting on response
					//pop(&f1); // pop off message from fifo
				//}
				//outStandingCmds=0; // command sent
			//}

		
		
		/* received message from slave logic */
		if (recievedResp) { // handle response
			TIMER0_disable(); 
			waitingForResp = 0;
			recievedResp=0; //handled response
			
			if (msgRespStatus < 0){ // local error
				// retransmit command
				restartTransaction();
				clearReceiveBuffer();
				// set other error output bits
			}
			///* COMMENT OUT IF TESTING RANDOM/ALL MSG VALUES!!!!! */
			//else if ( msgResp == slaveFrameError || msgResp == slaveDataOverRunError || msgResp == slaveFrameError ){ //slave error
				//// retransmit command
				//restartTransaction();
				//PORTA = ~PORTA;
			//}
			else { // if receive successful!
				timeout0Counter = 0;
				transmitUART0(outstandingMsg.address);
				transmitUART0(outstandingMsg.cmd);
				transmitUART0(msgResp); // not actually zero, verified with Abel, MAKE SURE MSG RESP IS SENDING '1'
				transmitUART0('#');
			}
			
		}
	}
}

ISR(USART0_RX_vect) {
	unsigned char temp = UDR0;
	if (temp == '!') {
		rstMsgTracker();
		//PORTA = ~PORTA;
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
