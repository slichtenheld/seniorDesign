/*
 * IncFile1.h
 *
 * Created: 11/11/2016 4:34:41 PM
 *  Author: samuel
 */ 


#ifndef USART_H
#define USART_H

#define NINEBITMODE 1


#define ADDRESS_MSG 1
#define DATA_MSG 0


#if defined (__AVR_ATmega324P__)
	#define INIT_CLOCK (DDRD |= (1 << DDRD4))
	#define STOP_CLOCK (DDRD &= ~(1 << DDRD4))
	#define SLAVE 0
#elif defined (__AVR_ATmega644P__)
	#define INIT_CLOCK (DDRD |= (1 << DDRD4))
	#define STOP_CLOCK (DDRD &= ~(1 << DDRD4))
	#define SLAVE 0
#else 
	#define INIT_CLOCK  // DDRA |= (1 << DDRA6), IF SLAVE DON'T INITIATE CLOCK!
	#define STOP_CLOCK  //DDRA &= ~(1 << DDRA6)
	#define SLAVE 1
#endif

#define BAUD 100
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)



void USART1_transmitEnable();
void USART1_transmitDisable();
void USART1_receiveEnable();
void USART1_receiveDisable();

void USART1_MPCM_on();

// UART1 RX Functions
int8_t USART1_receive(unsigned char *message) {
	
	//wait for message to be received
	//while(! (UCSR1A & (1 << RXC1)) ); not needed for interrupts
	
	int8_t errtemp = 0;
	if (UCSR1A & (1 << FE1)) errtemp = -1; // check for frame error
	else if (UCSR1A & (1 << DOR1)) errtemp = -2; // check for Data OverRun Error
	else if (UCSR1A & (1 << UPE1)) errtemp = -3; // check for Parity Error
		
	*message = UDR1; //reading UDR1 should clear error flags, doesn't appear to actually do so
	
	return errtemp;
}

// reset receive buffer, for some reason if multiple missed reads occur timeout no longer functions (has to do with RXC1 bit)
void clearReceiveBuffer(void) {
	unsigned char dummy;
	while ( UCSR1A & (1<<RXC1) ) dummy = UDR1;
	USART1_receiveDisable();
	USART1_receiveEnable();
};

// UART1 TX Functions
void USART1_transmit(unsigned char data, bool transmitMode) { //IF ISSUES CHECK DEFINES FOR INIT_CLOCK ETC AT TOP OF FILE
	
	/* Wait for empty transmit buffer*/
	while( !(UCSR1A & (1 << UDRE1)) ); 
	
	/* Copy 9th bit to TXB8, 9th bit is transmitMode */ 
	UCSR1B &= ~(1<<TXB81); // clear TXB81 first
	if ( transmitMode ) UCSR1B |= (1<<TXB81); // if address Mode, write a one to TXB81
	
	UDR1 = data;
	
	//The TXC Flag (UCSRnA.TXC)
	//while( !(UCSR1A & (1 << TXC1)) ); // Wait until transmit finished - OPTIONAL
	//UCSR1A |= (1 << TXC1);
	
}

// UART1 Initialization
void USART1_init(void) {
	UBRR1H = (unsigned char)(BAUDRATE >> 8);
	UBRR1L = (unsigned char)(BAUDRATE);
	
	UCSR1C = (1<<UMSEL10) | (3<<UCSZ10)| (1<<USBS1) | (3<<UPM10); // synchronous operation and 9-bit word, 2 stop bits, odd parity
	UCSR1B |= (1<<UCSZ12) ; // NEED THIS FOR 9-BIT WORD, UCSZ1[0..2] SPLIT AMONG THESE 2 REGS
	
	/* if slave module, turn on multiMaster mode */
	#if SLAVE
	USART1_MPCM_on();
	#endif
	
	// enable TX/RX pins
	USART1_transmitEnable();
	USART1_receiveEnable();
	INIT_CLOCK;
	
}

void USART1_MPCM_on(){
	UCSR1A |= (1<<MPCM1);
}
void USART1_MPCM_off(){
	UCSR1A &= ~(1<<MPCM1);
}


void USART1_transmitEnable(){
	UCSR1B |= (1 << TXEN1) ;
}
void USART1_transmitDisable(){
	UCSR1B &= ~(1 << TXEN1) ;
}

void USART1_receiveEnable(){
	UCSR1B |= (1 << RXEN1);
}
void USART1_receiveDisable(){
	UCSR1B &= ~(1 << RXEN1);
}
	
void USART1_receiveInterruptEnable(){
	// enable RX interrupt
	UCSR1B |= (1 << RXCIE1);
}

void USART1_receiveInterruptDisable(){
	UCSR1B &= ~(1 << RXCIE1);
}

void USART1_transmitInterruptEnable(){
	UCSR1B |= (1 << TXCIE1);
}




#endif // USART_H