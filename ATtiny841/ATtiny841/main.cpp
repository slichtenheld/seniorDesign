/*
 * ATtiny841.cpp
 *
 * Created: 10/27/2016 7:02:27 PM
 * Author : samuel
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "C:\Users\samuel\Documents\github\seniorDesign\myHeaderFiles\usart.h"
#include "C:\Users\samuel\Documents\github\seniorDesign\myHeaderFiles\timer1.h"
#include "C:\Users\samuel\Documents\github\seniorDesign\myHeaderFiles\messages.h"


void dispense();
void returnStatus();
void invalidCommand();
void stepperPortOutput();
void fullRotation();


//GLOBAL VARIABLES

volatile unsigned char status = STATUSOK;
//int step[] = {0b1100,0b0100, 0b0110,0b0010,0b0011,0b0001,0b1001, 0b1000}; // delay 8 ms
//int fullStep[] = {0b1100,0b0110,0b0011,0b1001}; // delay 15 ms
	//
//unsigned char test[]= {0b0,0b1,0b10,0b100,0b1000,0b10000,0b100000,0b10000000};

volatile int8_t outstandingRqsts;
volatile unsigned char msgRqst; // implement circular buffer?
volatile int8_t msgStatus;	  // receive status

volatile bool mode; 
#define MPCM_MODE 1// Multi-processor Communication mode - only stores data in receive buffer if 9th bit is set (address word)
#define DATA_MODE 0
volatile const unsigned char slaveAddr = 'X'; // use X, Y, and Z

int main(void)
{
	// initialize PORTB as outputs for debug purposes
	DDRB = 0b111;
	PORTB = 0b111;
	fullRotation();
	//PORTB = 0;
	//DDRA = 0b1111;
	//stepperPortOutput();
	/* Replace with your application code */
	USART1_init();
	USART1_receiveInterruptEnable();
	sei();
	
	msgStatus = 0;	  // receive status
	outstandingRqsts = 0;
	msgRqst = 0;
	mode = MPCM_MODE;
	PORTB = 0;
	PORTB = 7;
	
	while(1) { // infinite loop checking for outstanding requests
		
		//PORTA |= (outstandingRqsts << DDRB2); //somehow fixes code....
		
		//if (mode) PORTB |= (1<<DDRB2);
		//else PORTB &= ~(1<<DDRB2);
		
		
		if (outstandingRqsts>0) { // if outstanding request, handle request
			// CHECK IF BUSY --> SEND BUSY RESPONSE
			
			PORTB &= ~(1<<DDRB2);
			switch (msgStatus){
				case (FRAMEERROR):
					//clearReceiveBuffer();
					//PORTB &= ~(1<<DDRB2);
					USART1_transmit(slaveFrameError, DATA_MSG);
					break;
				case (DATAOVERRUNERROR):
					//clearReceiveBuffer();
					//PORTB &= ~(1<<DDRB1);
					USART1_transmit(slaveDataOverRunError, DATA_MSG);
					break;
				case (PARITYERROR):
					//clearReceiveBuffer();
					//PORTB &= ~(1<<DDRB0);
					USART1_transmit(slaveParityError, DATA_MSG);
					break;
				default: // no receive error
						//PORTB = 0b111;
						//PORTB = 0;
						switch(msgRqst){
							case (DISPENSE):
								fullRotation();
								USART1_transmit(DISPENSECOMPLETE, DATA_MSG);
								//PORTB &= ~(1<<DDRB0);
								break;
							case (STATUS):
								USART1_transmit(STATUSOK, DATA_MSG);
								//PORTB &= ~(1<<DDRB1);
								break;
							default:
								USART1_transmit(COMMANDINVALID , DATA_MSG);
						}
			}
			outstandingRqsts = 0;
		}
	}
}

ISR(USART1_RX_vect) {
	unsigned char msgTemp;
	volatile int8_t errorTemp = 0;
	errorTemp = USART1_receive(&msgTemp);
	clearReceiveBuffer();
	//if (mode==MPCM_MODE){
		//// check for address match
	//
		////if (errorTemp==FRAMEERORR || errorTemp==DATAOVERRUNERROR || errorTemp==PARITYERROR) { // if error message, reset receive buffer
			////clearReceiveBuffer();
		////}
		//if (msgTemp==slaveAddr){ // otherwise, see if MCU is trying to talk to this slave
			//USART1_MPCM_off();
			//mode=DATA_MODE;
		//}		 
	//}
	if (mode==MPCM_MODE)
	{
		// check for address match
		
		if (msgTemp==slaveAddr){ // otherwise, see if MCU is trying to talk to this slave
			USART1_MPCM_off();
			mode=DATA_MODE;
		}
		else { // if error message, reset receive buffer
			clearReceiveBuffer();
		}
	}
	else{ // normal data, actually care what it has to say
		outstandingRqsts = 1;
		msgStatus = errorTemp;
		msgRqst = msgTemp;
		
		mode = MPCM_MODE; // go back to listening for address
		USART1_MPCM_on();
	}
	
}

void stepperPortOutput(){
	// initialize PORTA[3..0] as outputs to drive motor driver
	DDRA |= 0b1111;
}

void stepperPortInput(){
	DDRA &= ~(0b1111); //make inputs so don't draw power
}

void fullRotation(){
	int fullStepB[] = {0b1100, 0b1001, 0b0011, 0b0110};
	//full rotation function
	stepperPortOutput(); // DDR* {1 for output pin, 0 for input pin}
	uint16_t currentLocation = 0;
	while(currentLocation < 500) {
		for (uint8_t i = 0; i < 4; i++) {
			PORTA = 0;
			PORTA = fullStepB[i];
			_delay_us(2000);
		}
		currentLocation++;
	}
	stepperPortInput();
}
