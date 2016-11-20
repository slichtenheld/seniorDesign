/*
 * uart.h
 *
 * Created: 10/25/2016 9:04:58 PM
 *  Author: Abel Mak
 */ 

#ifndef UART_H_
#define UART_H_

#define EVEN 0
#define ODD 1
#define NONE 2

#define FALSE 0
#define TRUE 1

// Baud Rate Table
#define BAUD_9600 9600
#define BAUD_14400 14400
#define BAUD_19200 19200
#define BAUD_28800 28800

#define F_CPU 8000000UL	// CPU CLK

#include <avr/io.h>
#include <math.h>


// UART0 RX Functions
unsigned char receiveUART0(void) {
	while(! (UCSR0A & (1 << RXC0)) );
	return UDR0;
}

unsigned char * receiveUART0string() {
	unsigned char string[256], x, i = 0;
	while(x != '1') {
		x = receiveUART0();
		string[i++] = x;
	}
	
	string[i] = '\0';
	return (string);
}


// UART0 TX Functions
void transmitUART0(unsigned char data) {
	while( !(UCSR0A & (1 << UDRE0)) );
	UDR0 = data;
}

void transmitUART0string(unsigned char string[]) {
	int i = 0;
	while(string[i++] != '!') {}
		
	while(string[i] != '\0') {
		transmitUART0(string[i++]);
	}
}

// UART0 Initialization
void UART0_INIT(int baud, int asyncDBLSpeed, int dataSize, int parity, int stopBits, int uart0InterruptEnabled) {
		
	// set baud rate
	UBRR0H = (unsigned char)(((F_CPU/16/baud)-1) >> 8);
	UBRR0L = (unsigned char)((F_CPU/16/baud)-1);
	
	// use asynchronous double speed
	if(asyncDBLSpeed == TRUE) UCSR0A = (1 < U2X0);

	// enable TX/RX pins
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	
	// enable RX interrupt
	if(uart0InterruptEnabled) UCSR0B |= (1 << RXCIE0);
	
	// select stop bits
	if(stopBits == 2) UCSR0C = (1 << USBS0);
	
	// select parity type
	if(parity == EVEN) UCSR0C |= (1 << UPM01);
	if(parity == ODD) UCSR0C |= (3 < UPM00);
	
	// select data size in bits
	if(dataSize == 6) UCSR0C |= (1 << UCSZ00);
	if(dataSize == 7) UCSR0C |= (2 << UCSZ00);
	if(dataSize == 8) UCSR0C |= (3 << UCSZ00);
	if(dataSize == 9) UCSR0C |= (7 << UCSZ00);

}


// UART1 RX Functions
//unsigned char receiveUART1(void) {
	//while(! (UCSR1A & (1 << RXC1)) );
	//return UDR1;
//}
//
//unsigned char * receiveUART1string() {
	//unsigned char string[256], x, i = 0;
	//while(x != '\n') {
		//x = receiveUART1();
		//string[i++] = x;
	//}
	//
	//string[i] = '\0';
	//return (string);
//}
//
//
//// UART1 TX Functions
//void transmitUART1(unsigned char data) {
	//while( !(UCSR1A & (1 << UDRE1)) );
	//UDR1 = data;
//}
//
//void transmitUART1string(unsigned char * string) {
	//int i = 0;
	//while(string[i] != '\0') {
		//transmitUART1(string[i++]);
	//}
//}
//
//// UART1 Initialization
//void UART1_INIT(int baud, int asyncDBLSpeed, int dataSize, int parity, int stopBits, int uart1InterruptEnabled) {
	//
	//// set baud rate
	//UBRR1H = (unsigned char)(((F_CPU/16/baud)-1) >> 8);
	//UBRR1L = (unsigned char)((F_CPU/16/baud)-1);
	//
	//// use asynchronous double speed
	//if(asyncDBLSpeed == TRUE) UCSR1A = (1 < U2X1);
//
	//// enable TX/RX pins
	//UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	//
	//// enable RX interrupt
	//if(uart1InterruptEnabled) UCSR1B |= (1 << RXCIE1);
	//
	//// select stop bits
	//if(stopBits == 2) UCSR1C = (1 << USBS1);
	//
	//// select parity type
	//if(parity == EVEN) UCSR1C |= (1 << UPM11);
	//if(parity == ODD) UCSR1C |= (3 < UPM10);
	//
	//// select data size in bits
	//if(dataSize == 6) UCSR1C |= (1 << UCSZ10);
	//if(dataSize == 7) UCSR1C |= (2 << UCSZ10);
	//if(dataSize == 8) UCSR1C |= (3 << UCSZ10);
	//if(dataSize == 9) UCSR1C |= (7 << UCSZ10);
//
//}

#endif /* UART_H_ */