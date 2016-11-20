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

#define OSC_CAL 0x59
#define BAUD_1 3000                                  
#define BAUDRATE ((F_CPU)/(BAUD_1*16UL)-1)

void clock_calibrate(); 
void restartTransaction();

// Abel's shitty interrupt vars
//volatile unsigned char rxByte0;
//unsigned char rxBuf0[256];
//volatile unsigned rxIndex0 = 0;
//volatile unsigned rxStringRdy0 = 0;
//
//volatile unsigned char rxByte1;
//unsigned char * rxBuf1;
//volatile unsigned rxIndex1 = 0;
//volatile unsigned rxStringRdy1 = 0;


unsigned char msgCmd; //uart interrupt will put data here
//uint8_t msgID; // value used to keep track of messages
int8_t msgCmdStatus;
int8_t outStandingCmds;
int8_t waitingForResp; // increment when waiting for response
bool timeout; // flag for timeouts

unsigned char msgResp; // response from module via usart
int8_t msgRespStatus;
int8_t recievedResp; // increment when receive responses

int main(void)
{
	// initialize test LED ports
	DDRA = 0xFF; // initialize to test
	DDRC = (1 << DDRC0);
	
	//calibrate clock
	clock_calibrate();
	
	// initialize uart to talk to esp
	UART0_INIT(BAUD_9600, FALSE, 8, NONE, 1, TRUE);
	
	// initialize timer
	TIMER1_init();
	TIMER1_compareInterruptEnable();
	
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
	timeout = false;
	
	while(1){
		
		//if(timeout) PORTA = ()
		
		if(timeout) { // handle timeout 
			restartTransaction();
			timeout = false; //clear timeout flag
			//USART1_receiveDisable();
		}
		
		//Simulating incoming commands via uart interrupt
		//else if (!outStandingCmds && !waitingForResp){ // if no outstanding commands and no outstanding responses
			//msgCmd = DISPENSE;
			//outStandingCmds=1;
		//}
		else if (m1Full && !outStandingCmds && !waitingForResp) { // handle message from ESP
			// handle address
			// m1.address Multi Processor Communication needed
			// set up communication to correct slave
			
			msgCmd = m1.cmd;
			outStandingCmds=1;
			rstMsgTracker();
		}
		
		
		else if (outStandingCmds && !waitingForResp) { // if there is an outstanding command and not waiting on a response
			
			if (m1.validity=='!'){
				USART1_transmit(msgCmd);
				TIMER1_enable();// set timer interrupt for how long to wait for response
				waitingForResp=1; // waiting on response
			}
	
			outStandingCmds=0; // command sent
			
			
		}
		
		else if (recievedResp) { // handle response
			TIMER1_disable(); 
			waitingForResp = 0;
			if (msgRespStatus < 0){ // local error
				// retransmit command
				
				restartTransaction();
				clearReceiveBuffer();
				// set other error output bits
			}
			else if ( msgResp == slaveFrameError || msgResp == slaveDataOverRunError || msgResp == slaveFrameError ){ //slave error
				// retransmit command
				restartTransaction();
			}
			else {
				PORTC ^= (1 << DDRC0);
				//PORTA = msgResp; // for now just echoing response to LEDs
			}
			recievedResp=0; //handled response
		}
	}
}

ISR(USART1_RX_vect) { // interrupt for receive
	recievedResp=1;
	msgRespStatus = USART1_receive(&msgResp);
}
// this ISR is fired whenever a match occurs
// hence, toggle led here itself..
ISR (TIMER1_COMPA_vect){
    // toggle led here
	timeout = true;
}

ISR(USART0_RX_vect) {
	unsigned char temp = UDR0;
	if (temp == '!') {
		rstMsgTracker();
		PORTA = ~PORTA;
	}
	addCharToMsg(temp);
	//rxByte0 = UDR0;
	//
	//if(rxByte0 != '\r') {
		//rxBuf0[rxIndex0++] = rxByte0;
	//}
	//else {
		//rxStringRdy0 = 1;
		//rxBuf0[rxIndex0++] = '\r';
		//rxBuf0[rxIndex0] = '\n';
	//}
	
}

void restartTransaction(){
	outStandingCmds=1; //should cause last message to be resent
	waitingForResp=0; //no longer waiting for response, restarting transaction
}
// timer1 overflow
//ISR(TIMER1_OVF_vect) {
	 //process the timer1 overflow here
//}


void clock_calibrate(void) {
	OSCCAL = OSC_CAL; // CLK calibration
}

//// UART1 RX Functions with TIMEOUT
//int8_t receiveUSART1(unsigned char *message) {
	//DDRD = (0 << DDRD4); // de-initialize for clockout
	//uint32_t timer = 0;
	//uint8_t errtmp = 0;
	//while( !(UCSR1A & (1 << RXC1)) );// {
		////timer++;
		////if ( timer >= 512000 ){
			////errtmp = -1;
			////break;
		////}
	////}
	//// check for frame error
	//if (UCSR1A & (1 << FE1)) errtmp = -2;
	//
	//// check for Data OverRun Error
	//else if (UCSR1A & (1 << DOR1)) errtmp = -3;
	//
	//// check for Parity Error
	//else if (UCSR1A & (1 << UPE1)) errtmp = -4;
	//
	//*message = UDR1;
	//
	//return errtmp;
//}

// TIMEOUT IMPLEMENTED
//char uart_getc(void) {
	//uint16_t timer = 0;
	//while (!(UCSR0A & (1<<RXC0))) {
		//timer++;
		//if(timer >= 16000) return 0;
	//} // Wait for byte to arrive
	//return UDR0;
//}
