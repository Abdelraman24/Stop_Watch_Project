/*
 *                               STOP_WATCH_PROJECT
 *
 *
 *        Author: Abdelrahman Diaa-Eldein
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned char sec=0,min=0,hours=0;

void PORT_init(void);
void INT0_init(void);
void INT1_init(void);
void INT2_init(void);
void TIMER1_init(void);

ISR(INT0_vect)
{
	           /* this interrupt is to reset the stop watch*/

	sec=0;
	min=0;
	hours=0;
}
ISR(INT1_vect)
{
	                /* this interrupt is to pause the stop watch
	                   by stopping the timer_1 */
	   TCCR1B=0;
}
ISR(INT2_vect)
{
	/* this interrupt is to resume the stop watch by resuming timer_1 */

	TCCR1B =(1<<WGM12)|(1<<CS10)|(1<<CS12);
}
ISR(TIMER1_COMPA_vect)
{
	sec++;
	if(sec==60)
	{
		sec=0;
		min++;
	}
	if(min==60)
	{
		min=0;
		hours++;
	}
}

int main (void)
{
	SREG|=(1<<7);  // Global interrupt enable
	PORT_init();
	INT0_init();
	INT1_init();
	INT2_init();
	TIMER1_init();

	while (1)
	{
		PORTA =(1);                //enable the first 7-segment

		/*the following expression (PORTC & (0xF0)) | (sec % 10)
		  will print only the ONES digit of seconds on the first 7-segment */

		PORTC = (PORTC & (0xF0)) | (sec % 10);
		_delay_us(30);
		PORTA = (1<<1);                        //enable second 7-segment
		PORTC = (PORTC & (0xF0)) | (sec/10);  // print the tens digit of seconds
		_delay_us(30);
		PORTA = (1<<2);                           //enable third 7-segment
		PORTC = (PORTC & (0xF0)) | (min % 10);   // print ones digit of minutes
		_delay_us(30);
		PORTA = (1<<3);                          //enable forth 7-segment
		PORTC = (PORTC & (0xF0)) | (min/10);    // print tens degit of minutes
		_delay_us(30);
		PORTA = (1<<4);                             //enable fifth 7-segment
		PORTC = (PORTC & (0xF0)) | (hours %10);    // print ones digit of hours
		_delay_us(30);
		PORTA = (1<<5);                          //enable last 7-segment
		PORTC = (PORTC & (0xF0)) | (hours/10);  // print tens digit of hours
		_delay_us(30);

	}



}
void PORT_init(void)
{
	DDRC |=(0x0F);     /* first four pins at port C as output for*/
	PORTC &=~(0x0F);  /* 7447 decoder pins and put 0 as initial value*/

	DDRA |=(0x3F);     /* first six pins at port A as output to */
	PORTA &=~(0x3F);  /*(enable&disable)-7-segments and put 0 as initial value*/
}
void INT0_init(void)
{
	DDRD &=~(1<<PD2);   /* define the Interrupt 0 pin as input*/
	PORTD |=(1<<PD2);  /*and activate the internal pull up resistor*/

	GICR |=(1<<INT0);    // enable the interrupt 0
	MCUCR |=(1<<ISC01); // define INT0 as falling edge triggered
}
void INT1_init(void)
{
	DDRD &=~(1<<PD3);   /* define the Interrupt 1 pin as input with
	                       external pull down technique*/

	GICR |=(1<<INT1);    // enable the interrupt 1
	MCUCR = MCUCR |(1<<ISC10)|(1<<ISC11); // define INT1 as rising edge triggered
}
void INT2_init(void)
{
	DDRB &=~(1<<PB2);   /* define the Interrupt 2 pin as input*/
	PORTB |=(1<<PB2);  /*and activate the internal pull up resistor*/

	GICR |=(1<<INT2);    // enable the interrupt 2
	MCUCSR &=~(1<<ISC2); // define INT2 as falling edge triggered
}
void TIMER1_init(void)
{
	TCNT1=0;               // start the counter from 0

	TCCR1A =(1<<FOC1A);   /*The FOC1A bit are only active when
	                        the timer work on a non-PWM mode.*/

	TCCR1B =(1<<WGM12)|(1<<CS10)|(1<<CS12);
	             /* Timer1 with CTC mode with prescaler equal "1024" */

	TIMSK |=(1<<OCIE1A);  // Output Compare A Match Interrupt Enable

	OCR1A=800;                    /* set compare value to "970" ,
	                         to reach one second every compare match*/
}

