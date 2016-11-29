/*
 * IncFile1.h
 *
 * Created: 11/12/2016 12:27:48 PM
 *  Author: samuel
 */ 



#ifndef TIMER1_H
#define TIMER1_H

#include <util/atomic.h> // might not be needed


#define COMPARE_VAL0 255 //value for stepper motor, should use ~500 for normal cmds
#define COMPARE_VAL1 50000

void TIMER1_enable();
void TIMER1_disable();
void TIMER1_compareInterruptEnable();
void TIMER1_init();
void TIMER1_WriteTCNT1( unsigned int i );

void TIMER0_enable();
void TIMER0_disable();
void TIMER0_compareInterruptEnable();
void TIMER0_init();
void TIMER0_WriteTCNT0( unsigned int i );

void TIMER1_enable(){
	TIMER1_WriteTCNT1(0);
	TCCR1B |= (1 << CS10);
	TCCR1B |= (1 << CS12);
}

void TIMER1_disable(){ // disable clock source
	TCCR1B &= ~(1 << CS10);
	TCCR1B &= ~(1 << CS11);
	TCCR1B &= ~(1 << CS12);
}

void TIMER1_compareInterruptEnable(){
	// enable compare interrupt
	TIMSK1 |= (1 << OCIE1A);
}

void TIMER1_init(){
	
	// initialize counter
	TCNT1 = 0;
	
	// initialize compare value
	OCR1A = COMPARE_VAL1;
	
	// set up timer with prescaler = 1064 and CTC mode
	TCCR1B = (1 << WGM12);
	//TCCR1B = (1 << WGM12)|(1 << CS12)|(1 << CS10);
}

void TIMER1_WriteTCNT1( unsigned int i )  // CTC mode clears CNT1
{
	unsigned char sreg;
	/* Save global interrupt flag */
	sreg = SREG;
	/* Disable interrupts */
	cli();
	/* Set TCNT1 to i */
	TCNT1 = i;
	/* Restore global interrupt flag */
	SREG = sreg;
}
// ******************  //
// TIMER0 8-bit Timer  //
// ******************  //

void TIMER0_enable(){
	TIMER0_WriteTCNT0(0);
	TCCR0B |= (1 << CS00);
	TCCR0B |= (1 << CS02);
}

void TIMER0_disable(){ // disable clock source
	TCCR0B &= ~(1 << CS00);
	TCCR0B &= ~(1 << CS01);
	TCCR0B &= ~(1 << CS02);
}

void TIMER0_compareInterruptEnable(){
	// enable compare interrupt
	TIMSK0 |= (1 << OCIE0A);
}

void TIMER0_init(){
	
	// initialize counter
	TCNT0 = 0;
	
	// initialize compare value
	OCR0A = COMPARE_VAL0;
	
	// set up timer with prescaler = 1064 and CTC mode
	TCCR0B = (1 << WGM02);
	//TCCR1B = (1 << WGM12)|(1 << CS12)|(1 << CS10);
}

void TIMER0_WriteTCNT0( unsigned int i )  // CTC mode clears CNT1
{
	unsigned char sreg;
	/* Save global interrupt flag */
	sreg = SREG;
	/* Disable interrupts */
	cli();
	/* Set TCNT1 to i */
	TCNT0 = i;
	/* Restore global interrupt flag */
	SREG = sreg;
}

#endif /* TIMER1_H */

//void TIMER1_overflowInterruptEnable(){
	//// enable timer overflow interrupt for Timer1
	//TIMSK1 |= (1<<TOIE1);
//}