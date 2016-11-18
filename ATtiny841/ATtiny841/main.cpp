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


void dispense();
void returnStatus();
void invalidCommand();
void stepperPortOutput();
void fullRotation();


//GLOBAL VARIABLES

const unsigned char address = 'x'; // use x, y, and z
unsigned char status = STATUSOK;
//int step[] = {0b1100,0b0100, 0b0110,0b0010,0b0011,0b0001,0b1001, 0b1000}; // delay 8 ms
//int fullStep[] = {0b1100,0b0110,0b0011,0b1001}; // delay 15 ms
int fullStepB[] = {0b1100, 0b1001, 0b0011, 0b0110};
	//
//unsigned char test[]= {0b0,0b1,0b10,0b100,0b1000,0b10000,0b100000,0b10000000};

int8_t outstandingRqsts;
unsigned char msgRqst; // implement circular buffer?
int8_t msgStatus;	  // receive status


int main(void)
{
	// initialize PORTB as outputs for debug purposes
	DDRB = 0b111;
	//DDRA = 0b1111;
	//stepperPortOutput();
	/* Replace with your application code */
	USART1_init();
	USART1_receiveInterruptEnable();
	sei();
	
	msgStatus = 0;	  // receive status
	outstandingRqsts = 0;
	msgRqst = 0;
	fullRotation();
	
	
	while(1) { // infinite loop checking for outstanding requests
		
		PORTB = (outstandingRqsts << DDRB0); //somehow fixes code....
		
		if (outstandingRqsts) { // if outstanding request, handle request
			// CHECK IF BUSY --> SEND BUSY RESPONSE
			switch (msgStatus){
				case (FRAMEERORR):
					clearReceiveBuffer();
					PORTB = (1<<DDRB0);
					USART1_transmit(slaveFrameError);
					break;
				case (DATAOVERRUNERROR):
					clearReceiveBuffer();
					PORTB = (1<<DDRB1);
					USART1_transmit(slaveDataOverRunError);
					break;
				case (PARITYERROR):
					clearReceiveBuffer();
					PORTB = (1<<DDRB2);
					USART1_transmit(slaveParityError);
					break;
				default: // no receive error
						switch(msgRqst){
							case (DISPENSE):
								dispense();
								break;
							case (STATUS):
								returnStatus();
								break;
							default:
								invalidCommand();
						}
			}
			outstandingRqsts--;
		}
	}
}

ISR(USART1_RX_vect) {
	msgStatus = USART1_receive(&msgRqst);
	outstandingRqsts ++;
	PORTA = (outstandingRqsts << DDRA0);
}

void dispense(){
	fullRotation();
	USART1_transmit(DISPENSECOMPLETE);
}

void returnStatus(){
	USART1_transmit(STATUSOK);
}

void invalidCommand(){
	USART1_transmit(COMMANDINVALID);
}

void stepperPortOutput(){
	// initialize PORTA[3..0] as outputs to drive motor driver
	DDRA |= 0b1111;
}

void stepperPortInput(){
	DDRA &= ~(0b1111); //make inputs so don't draw power
}

void fullRotation(){
	//full rotation function
	stepperPortOutput(); // DDR* {1 for output pin, 0 for input pin}
	uint16_t currentLocation = 0;
	while(currentLocation < 512) {
		for (uint8_t i = 0; i < 4; i++) {
			PORTA = 0;
			PORTA = fullStepB[i];
			_delay_us(2000);
		}
		currentLocation++;
	}
	stepperPortInput();
}
