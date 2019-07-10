/*
 * Lab10-ConcurrentSynchMachine.c
 *
 * Created: 7/10/2019 3:58:30 PM
 * Author : Taft
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <timer.h>

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B 	= 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect)
{
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

enum ThreeLEDsSM {BitZero, BitOne, BitTwo} threeLEDs;
enum BlinkingLEDSM {BitThree, Zero} blinkingLED;
enum CombineLEDsSM {Combination} combineLED;

void Tick();

int main(void)
{
	DDRB = 0xFF;
	PORTB = 0x00;
	TimerSet(1); 
	TimerOn();  
	TimerFlag = 0;
	threeLEDs = BitZero;
	blinkingLED = BitThree;
	combineLED = Combination;
    while (1) 
    {
		Tick();
		while(!TimerFlag){};
		TimerFlag = 0;
    }
}

void Tick(){
	unsigned char LEDsBehavior = 0x00;
	unsigned char BlinkingBehavior = 0x00;
	static int TimeCtr = 0;
	
	switch(threeLEDs){
		case BitZero:
			LEDsBehavior = 0x01; 
			if(TimeCtr == 1000){ 
				threeLEDs = BitOne;
			}
			break;
		case BitOne:
			LEDsBehavior = 0x02; 
			if(TimeCtr == 1000){ 
				threeLEDs = BitTwo;
			}
			break;
		case BitTwo:
			LEDsBehavior = 0x04; 
			if(TimeCtr == 1000){ 
				threeLEDs = BitZero;
			}
			break;
		default: 
			threeLEDs = BitZero;
	}

	switch(blinkingLED){
		case BitThree:
			BlinkingBehavior = 0x00;
			if(TimeCtr == 1000){ 
				blinkingLED = Zero;
			}
			break;
		case Zero:
			BlinkingBehavior = 0x08;
			if(TimeCtr == 1000){ 
				blinkingLED = BitThree;
			}
			break;
		default:
			blinkingLED = BitThree;
	}
	
	switch(combineLED){
		case Combination:
			if(TimeCtr == 1000){ 
				PORTB = (LEDsBehavior | BlinkingBehavior); 
			}
			break;
		default:
			combineLED = Combination;
	}
	
	if(TimeCtr < 1000){
		TimeCtr++; 
	}
	else{
		TimeCtr = 0;
	}
}