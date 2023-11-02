/*
 * main.c
 *Formål: Hovedfil 
* Created: 11-01-2021 13:56:11
 * Author : paw94 & udhansen
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "ssd1306.h"
#include "I2C.h"
#include "buttons.h"
#include "display.h"
#include "sampleADC.h"
#include "stdio.h"

#define F_CPU 16000000UL
#include <math.h>
#include <string.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

enum state{init,st1,st2,st3,st4,st5,st6,st7,st8,st9,st10};
char state=init;

float adc_buffer[64];
int signal_count = 0;
int i = 0;

float Rsum = 0;
float Isum = 0;
float Re = 0;
float Im = 0;
float mod = 0;
float arg = 0;
float ampOffSet = 0;
float phaseOffSet = 0;
float mod_prev = 0;
float arg_prev = 0;

uint8_t _i2c_address;
uint8_t display_buffer[1024];
	
//used for test
const uint8_t test_bmp[] PROGMEM =
{0x00,0x41,0x22,0x14,0x08,0x00,0x00,0x00};

//used for test
const uint8_t logo16_glcd_bmp[] PROGMEM =
{ 0b00000000, 0b11000000,
	0b00000001, 0b11000000,
	0b00000001, 0b11000000,
	0b00000011, 0b11100000,
	0b11110011, 0b11100000,
	0b11111110, 0b11111000,
	0b01111110, 0b11111111,
	0b00110011, 0b10011111,
	0b00011111, 0b11111100,
	0b00001101, 0b01110000,
	0b00011011, 0b10100000,
	0b00111111, 0b11100000,
	0b00111111, 0b11110000,
	0b01111100, 0b11110000,
	0b01110000, 0b01110000,
0b00000000, 0b00110000 };

void calc_dft(); // funktion som laver vores dft

int main(void)
{
	_i2c_address = 0X78;
	I2C_Init();
	InitializeDisplay();
	ADC_init();
	sei();
	int click_state = 0;
	DDRB |= (1<<PB6);
	
	while (1)
	{		
		switch(state) {
			case init: // initilizer state: Sætter display, knapper og timer
				I2C_Init(); 
				_delay_ms(10); 
				print_fonts();
				_delay_ms(500); 
				disp_setup(); 
				but_setup();
				Timer1();
				ampOffSet = 0;
				phaseOffSet = 0; 
				state=st1; 
				break;
			
			case st1:  state=st1; break; // sampling state: Her vil timer kører indtil det registreres at bufferen er fuld.
			
			case st2:  // calc state: Bufferen er fuld og der laves dft. Phase og Amp hentes af display funktionen og printes til OLED
				PORTB &=~ (1<<PB6);
				calc_dft();
				mod_prev = mod;
				arg_prev = arg;
				disp_update(mod-ampOffSet, arg-phaseOffSet, ampOffSet);
				state=st3;
				break;
				
			case st3: // kontrol state: Her registres hvis knapperne bliver trykket og hvad der skal gøres hvis en trykkes
				click_state = but_clicks();
				
				if(click_state ==1){state=st4;} // stopper og går til st4 
				else if(click_state == 2) {state=init;} // reset som går tilbage og initilizer det hele igen
				else if(click_state == 3)  // vores offset som nulstiller
				{
					ampOffSet = mod;
					phaseOffSet = arg;	
				}
				else {state=st1;}
				break;
				
			case st4: // stop state: Det er stoppet indtil der enten trykkes på start/stop eller reset
				setXY(1,3);
				sendStr("stopped     ");
				click_state = but_clicks();
				
				if(click_state == 1) {state=st1;}
				else if(click_state == 3)
				{
					ampOffSet = mod;
					phaseOffSet = arg;
				}
				else {state=st4;}
				break;
				
			default: break;
		}
		
	}
}ISR(TIMER1_COMPB_vect)
{

	TCNT1 = 0; // timer reset
	if(signal_count==4)
	{
		signal_count = 0;
	}
	if(signal_count < 2)
	{
		PORTB |= (1<<PB6);// tænd transmit signal
	}
	if(signal_count >= 2)
	{
		PORTB &=~ (1<<PB6);// sluk transmit signal
			
	}
	if (i == 63)
	{
		state=st2;
		i= 0;
	}
	ADCSRA |= (1<<ADSC);			// to start adc conversion
	while(!(ADCSRA & (1<<ADIF)));
	adc_buffer[i] = ADCH;			// gemmer hvad der er i ADCH i array
	ADCSRA &=~ (1<<ADSC);			// stop adc conversion

	i++;
	signal_count++;
}

ISR(ADC_vect)
{	
	
}

void calc_dft()
{

	int j=0;
	int N=64;
	float coss[] = {1,0,-1,0}; // Cosinus tal sekvens
	float sins[] = {0,1,0,-1}; // Sinus tal sekvens
	Re = 0;
	Im = 0;

	for(int k = 0; k < N; k++)
	{

		Re += adc_buffer[k] * coss[j];		// Udregner den reelle værdi af buffer værdien
		Im -= adc_buffer[k] * sins[j];		// Udregner den imaginære værdi af buffer værdien

		if(j==3)	// If statement der kører igennem cosinus værdierne og sinus værdierne.
		{
			j = 0;
		} else
		{
			j++;
		}

	}

	
	
	mod = sqrt(Re*Re+Im*Im); //Modulus omregning
	arg = atan2(Im,Re)*180/3.14; 	//Argument omregning 180/3.14 er for at få det i grader i stedet for radianer.
	
	// IIR Filter
	mod = mod * 0.9 + 0.1 * mod_prev;
	arg = arg * 0.9 + 0.1 * arg_prev;
	


}
