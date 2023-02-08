#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define DELAY 2
/* global variable contain the ticks count of the timer */
unsigned char g_second_1=0;
unsigned char g_second_2=0;
unsigned char g_minute_1=0;
unsigned char g_minute_2=0;
unsigned char g_hour_1=0;
unsigned char g_hour_2=0;

/* Interrupt Service Routine for timer1 compare mode */
ISR(TIMER1_COMPA_vect)
{
		g_second_1++;

		if(g_second_1 == 10)
		{
			g_second_1 = 0;
			g_second_2++;
		}
		if(g_second_2 == 6)
		{
			g_second_2 = 0;
			g_minute_1++;
		}
		if(g_minute_1 == 10)
		{
			g_minute_1=0;
			g_minute_2++;
		}
		if(g_minute_2 == 6)
		{
			g_minute_2 = 0;
			g_hour_1++;
		}
		if(g_hour_1 == 10)
		{
			g_hour_1 = 0;
			g_hour_2++;
		}
}
ISR(INT0_vect)
{
	// Reseting all the counters to start from the begining

	g_second_1 = 0;
	g_second_2 = 0;
	g_minute_1 = 0;
	g_minute_2 = 0;
	g_hour_1 = 0;
	g_hour_2 = 0;
}
ISR(INT1_vect)
{
	// Disabling the clock therefore there will be no timer counting

	TCCR1B &= ~(1<<CS10) & ~(1<<CS11);
}

ISR(INT2_vect)
{
	// Enabling the clock again to make the timer count again

	TCCR1B |= (1<<CS10) | (1<<CS11);
}

void Timer1_CTC_Init(int ticks)
{
	TCNT1 = 0;		/* Set timer1 initial count to zero */

	OCR1A = ticks;
	TIMSK |= (1<<OCIE1A); /* Enable Timer1 Compare A Interrupt */
	TCCR1A = (1<<FOC1A);

	/* Timer1 is 16 bit so max counts= 65535
	 * with MU 1MHZ then we need our Pre scaler to be 64 that produce interrupt every ~4.2 sec with max value
	 * we need to produce interrupt every 1 Sec so we will set our compare value to ~15640
	 * Pre scaler = F_CPU/64 CS10=1 CS11=1 CS12=0
	  count up every 1 second*/

	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS11);
}
void INT0_Init()
{
	// Enabling INT0

	GICR |= (1<<INT0);
	MCUCR |= (1<<ISC01); //Generate Interrupt with falling edge
	MCUCR &= ~(1<<ISC00);
	DDRD &= ~(1<<PD2); // Configure pin 2 in PORTD as input pin
	PORTD |= (1<<PD2); // Activate the internal pull up resistor at PD2
}


void INT1_Init()
{
	// Enabling INT1

	GICR |= (1<<INT1);
	MCUCR |= (1<<ISC10) | (1<<ISC11); //Generate Interrupt with rising edge
	DDRD &= ~(1<<PD3); // Configure pin 3 in PORTD as input pin
}


void INT2_Init()
{
	// Enabling INT2

	GICR |= (1<<INT2);
	MCUCSR &= ~(1<<ISC2); //Generate Interrupt with falling edge
	DDRB &= ~(1<<PB2); // Configure pin 2 in PORTB as input pin
	PORTB |= (1<<PB2); // Activating the internal pull up resistor at PB2
}



int main(void){
	DDRC |= 0x0F;  // Making the first 4 pins in PORTC output pins
	DDRA |= 0x3F;  // Making the first 6 pins in PORTA output pins

	PORTC &= ~0x0F; // Initializing the OUTPUT of First 4 PINS in PORTC with 0
	PORTA &= ~0x3F; // Initializing the OUTPUT of First 6 PINS in PORTA with 0

	SREG |= (1<<7); /* Enable global interrupts in MC */

	INT0_Init();    // Initializing INT0
	INT1_Init();    // Initializing INT1
	INT2_Init();    // Initializing INT2

	Timer1_CTC_Init(62); // Calling our Timer to start counting

	while(1){

		PORTA = (1<<PA0);
		PORTC = (PORTC & 0xF0) | (g_second_1 & 0x0F);
		_delay_ms(DELAY);


		PORTA = (1<<PA1);
		PORTC = (PORTC & 0xF0) | (g_second_2 & 0x0F);
		_delay_ms(DELAY);


		PORTA = (1<<PA2);
		PORTC = (PORTC & 0xF0) | (g_minute_1 & 0x0F);
		_delay_ms(DELAY);


		PORTA = (1<<PA3);
		PORTC = (PORTC & 0xF0) | (g_minute_2 & 0x0F);
		_delay_ms(DELAY);


		PORTA = (1<<PA4);
		PORTC = (PORTC & 0xF0) | (g_hour_1 & 0x0F);
		_delay_ms(DELAY);


		PORTA = (1<<PA5);
		PORTC = (PORTC & 0xF0) | (g_hour_2 & 0x0F);
		_delay_ms(DELAY);
	}
}
