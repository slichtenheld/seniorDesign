/*
 * IncFile1.h
 *
 * Created: 11/11/2016 4:34:41 PM
 *  Author: samuel
 */ 


#ifndef USART_H
#define USART_H


#if defined (__AVR_ATmega324P__)
	#define INIT_CLOCK (DDRD |= (1 << DDRD4))
	#define STOP_CLOCK (DDRD &= ~(1 << DDRD4))
#else 
	#define INIT_CLOCK  // DDRA |= (1 << DDRA6), IF SLAVE DON'T INITIATE CLOCK!
	#define STOP_CLOCK  //DDRA &= ~(1 << DDRA6)
#endif

#define BAUD 500
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)

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

#define FRAMEERORR -1
#define DATAOVERRUNERROR -2
#define PARITYERROR -3

void USART1_transmitEnable();
void USART1_transmitDisable();
void USART1_receiveEnable();
void USART1_receiveDisable();

// UART1 RX Functions
int8_t USART1_receive(unsigned char *message) {
	
	//wait for message to be received
	//while(! (UCSR1A & (1 << RXC1)) ); not needed for interrupts
	
	int8_t errtemp = 0;
	if (UCSR1A & (1 << FE1)) errtemp = -1; // check for frame error
	else if (UCSR1A & (1 << DOR1)) errtemp = -2; // check for Data OverRun Error
	else if (UCSR1A & (1 << UPE1)) errtemp = -3; // check for Parity Error
	
	*message = UDR1; //reading UDR1 clears error flags
	
	return errtemp;
}

// reset receive buffer, for some reason if multiple missed reads occur timeout no longer functions (has to do with RXC1 bit)
void clearReceiveBuffer(void) {
	USART1_receiveDisable();
	USART1_receiveEnable();
};

// UART1 TX Functions
void USART1_transmit(unsigned char data) { //IF ISSUES CHECK DEFINES FOR INIT_CLOCK ETC AT TOP OF FILE
	
	
	while( !(UCSR1A & (1 << UDRE1)) );
	UDR1 = data;
	
	//The TXC Flag (UCSRnA.TXC)
	while( !(UCSR1A & (1 << TXC1)) ); // Wait until transmit finished - OPTIONAL
	UCSR1A |= (1 << TXC1);
	
}

// UART1 Initialization
void USART1_init(void) {
	UBRR1H = (unsigned char)(BAUDRATE >> 8);
	UBRR1L = (unsigned char)(BAUDRATE);
	UCSR1C= (1<<UMSEL10) | (3<<UCSZ10)| (1<<USBS1) | (3<<UPM10); // synchronous operation and 8-bit word, 2 stop bits, odd parity

	// enable TX/RX pins
	USART1_transmitEnable();
	USART1_receiveEnable();
	INIT_CLOCK;
	
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

// When the frame type bit (the first stop or the ninth bit) is one, the frame contains an
// address. When the frame type bit is zero the frame is a data frame.

void USART_Transmit( unsigned int data ) //9 bit frame
{
	 /* The TXC Flag (UCSRnA.TXC) can be used to
	 check that the Transmitter has completed all transfers, and the RXC Flag can be used to check that there
	 are no unread data in the receive buffer. The UCSRnA.TXC must be cleared before each transmission
	 (before UDRn is written) if it is used for this purpose. */
	 
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1))); // TXC1???
	/* Copy 9th bit to TXB8 */
	UCSR1B &= ~(1<<TXB81);
	if ( data & 0x0100 )
		UCSR1B |= (1<<TXB81);
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

unsigned int USART_Receive( void ) 
{
	unsigned char status, resh, resl;
	/* Wait for data to be received */
	while ( !(UCSR1A & (1<<RXC1)) );
	/* Get status and 9th bit, then data */
	/* from buffer */
	status = UCSR1A;
	resh = UCSR1B;
	resl = UDR1;
	/* If error, return -1 */
	if ( status & ((1<<FE1)|(1<<DOR1)|(1<<UPE1)) )
	return -1;
	/* Filter the 9th bit, then return */
	resh = (resh >> 1) & 0x01;
	return ((resh << 8) | resl);
}


#endif // USART_H