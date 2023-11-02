/*
 * display.c
 * 
 *Formål: Sætning og updatering af displayed 
 *Input: update bliver kaldt med parametrene for fase, amplitude og offset  
 *Output: Den data sendes til displayed.
 * Created: 11-01-2021 14:02:40
 *  Author: paw94 & udhansen
 */ 
#include "display.h" //!!! project2 header with functions made for this project!!!
#include "buttons.h"
#include "I2C.h"
#include "ssd1306.h"
#include "sampleADC.h"
#include <stdlib.h>
#include <avr/io.h>
#include <stdio.h>
#define F_CPU 16000000UL
#include <math.h>
#include <string.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

char amplitude[20]={0};
char phase[20]={0};
char ampOffSetDisp[20]={0};

void disp_setup()
{
	reset_display();// Initialize SSD1306 OLED display
	setXY(1,5);
	sendStr("Running  ");
	setXY(3,0);
	sendStr("Amp:"); 
	setXY(5,0);
	sendStr("Phase:"); 
	setXY(7,0);
	sendStr("Offset:");
}


void disp_update(float mod, float arg, float ampOffSet)
{
	// PULL UPDATE TO DISPLAY
	
	setXY(3,8);
	dtostrf(mod,4,2,amplitude);

	//sprintf(amplitude,"%f   ",mod);
	sendStr(amplitude);

	setXY(5,8);
	dtostrf(arg,4,2,phase);
	//sprintf(phase,"%f   ",arg);
	sendStr(phase);
	
	setXY(7,8);
	dtostrf(ampOffSet,4,2,ampOffSetDisp);
	//sprintf(phase,"%f   ",arg);
	sendStr(ampOffSetDisp);
		
}
