/*
 * lcd.h
 *
 * Created: 10/25/2016 7:50:03 PM
 *  Author: Abel Mak
 */ 


#ifndef LCD_H_
#define LCD_H_

#define BITS8 0xFF
#define NULL 0

#include <avr/io.h>
#include <util/delay.h>

/*
	RS
*/
#define Command (0 << PINB0)
#define Data (1 << PINB0)

/*
	E
*/
#define Enable (1 << PINB1)
#define Disable (0 << PINB1)

/*
	LCD Commands
*/
#define FunctionSet 0x38			// 2-Line Mode, Font OFF
#define DisplayCursorBlink 0x0F		// Display, Cursor, Blink ON
#define ClearHome 0x01				// Clear Screen, Return Home ON
#define EntryMode 0x07				// Shift and Increment by 1, ON
#define SetDDRAM 0xC0				// Set Cursor on 2nd Line

void LCD_INIT(void){
	DDRA = BITS8;						// 8-bit OUTPUT to LCD
	DDRB = (1 << PINB0) | (1 << PINB1); // Control Signals to LCD
	
	//// INIT Commands //
	PORTB = Enable | Command;
	_delay_us(400);
	PORTA = FunctionSet;
	_delay_us(400);
	PORTB = Disable | Command;
	_delay_us(400);
	
	PORTB = Enable | Command;
	_delay_us(400);
	PORTA = DisplayCursorBlink;
	_delay_us(400);
	PORTB = Disable | Command;
	_delay_us(400);
	
	PORTB = Enable | Command;
	_delay_us(400);
	PORTA = ClearHome;
	_delay_us(400);
	PORTB = Disable | Command;
	_delay_us(400);
	
	PORTB = Enable | Command;
	_delay_us(400);
	PORTA = EntryMode;
	_delay_us(400);
	PORTB = Disable | Command;
	_delay_us(400);
}

void LCD_Char(unsigned char letter){
	PORTB = Enable | Data;
	_delay_us(400);
	PORTA = letter;
	_delay_ms(400);
	PORTB = Disable | Data;
	_delay_us(400);
}

void LCD_ToString(char word[]){
	unsigned i = 0;
	while(word[i] != NULL){
		LCD_Char(word[i]);
		i++;
	}
}

void LCD_Clear(){
	
	_delay_ms(2000);
	
	PORTB = Enable | Command;
	_delay_us(400);
	PORTA = ClearHome;
	_delay_us(400);
	PORTB = Disable | Command;
	_delay_us(400);
}

void LCD_NextLine(){
	PORTB = Enable | Command;
	_delay_us(400);
	PORTA = SetDDRAM;
	_delay_us(400);
	PORTB = Disable | Command;
	_delay_us(400);
}



#endif /* LCD_H_ */