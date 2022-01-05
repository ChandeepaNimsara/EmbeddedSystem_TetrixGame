#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#define D7 eS_PORTD7
#define D6 eS_PORTD6
#define D5 eS_PORTD5
#define D4 eS_PORTD4
#define EN eS_PORTC1
#define RS eS_PORTC0

#define BUZZER PORTC

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"

int Start_game = 0;
int Continue_game = 0;
int i = 0;
int pin_buzzer = 0;

int main(void){
	
	DDRC = 0XFF;
	DDRB = 0X00;
	DDRD = 0XF0;         //0b11110000;
	
	MCUCR |= 1 <<ISC11 | 1 << ISC01;          //    0b00000010 , 0b00001000
	GICR = (1<<INT0)|(1<<INT1)|(1<<INT2);
	MCUCSR = 0x00;
	sei();
	Lcd4_Init();
	
	while(1){
		
		if(Start_game == 0){
			Lcd4_Set_Cursor(1,4);
			Lcd4_Write_String("HI TETRIS");
			Lcd4_Set_Cursor(2,2);
			Lcd4_Write_String("GAME START...");
			Lcd4_Clear();
		}
		else if(Start_game == 1 && Continue_game == 1){
			char score[10];
			sprintf(score,"%d",i);
			Lcd4_Set_Cursor(1,1);
			Lcd4_Write_String("SCORE: ");
			Lcd4_Write_String(score);
		}
		else if(Start_game == 1 && Continue_game == 0){
			char score[10];
			sprintf(score,"%d",i);
			Lcd4_Set_Cursor(1,1);
			Lcd4_Write_String("YOUR SCORE: ");
			Lcd4_Write_String(score);
			Lcd4_Set_Cursor(2,1);
			Lcd4_Write_String("START AGAIN...");
			Lcd4_Clear();
			
			BUZZER = 0X04 ;
			_delay_ms(100);
			
			BUZZER = 0X00;		
		}
	}
}

ISR(INT0_vect){
	i++;
}

ISR(INT1_vect){
	if(Start_game == 0){
		Start_game = 1;
		Continue_game = 1;
		i=0;
	}
	
	else if (Start_game == 1 && Continue_game == 0){
		Start_game = 1;
		Continue_game = 1;
		i=0;
	}
}

//get final score

ISR(INT2_vect){
	Continue_game = 0;
	
}

