/*
 * sampleADC.c
 *
 *Formål: Initilize adc og timer
 *Input: 
 *Output:
 * Created: 11-01-2021 14:09:34
 *  Author: paw94 & udhansen
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "I2C.h"
#include "stdio.h"
#include "sampleADC.h"
#define F_CPU 16000000UL
#include <math.h>
#include <string.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
int channel = 0;

void ADC_init()
{
	
	ADCSRA|=(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);   // sætter frekvensen til 125kHz ved en division factor 128
	ADCSRA|=(1<<ADEN);							// Enabler ADC'en
	ADCSRA|=(1<<ADATE) | (1<<ADIE);				//enabler Auto trigger mode
	ADCSRB|=(1<<ADTS2) | (1<<ADTS0);			// timer compare match B
	
	ADMUX = channel;
	ADMUX|=(1<<ADLAR);
	DIDR0 = (1<<channel);
	ADMUX |= (1<<REFS1) | (1<<REFS0);
	DIDR0 =! DIDR0; 			// disabler ubrugte digital porte for at unødvendig energi forbrug
	DIDR1 = 0xff;			// -||-
}
void Timer1(){
	TCCR1B |=(1<<CS11);		// Sætter prescaler factor til 8 så vi får den ønskede compare match value
	TCCR1B |=(1<<WGM12);	// Indstiller CTC mode
	OCR1A = 249;			// Compare match value sat til 249 for 1ms
	OCR1B = 249;
	TIMSK1|=(1<<OCIE1B);	// enabler timer interrupt
	TCNT1 = 0;				// timer reset
}
