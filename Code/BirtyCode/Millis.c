/*
 * Millis.c
 *
 * Created: 5/30/2022 5:36:32 PM
 *  Author: Jack2bs
 */ 

#include <avr/interrupt.h>
#include "ExtInt.h"
#include "Minimu9V5.h"

// Variable stores the number of milliseconds since last call to resetMillis()
int g_millis = 0;
int g_lastOVF = 32500;

// This interrupt increments the g_millis variable
ISR(TIMER0_COMPA_vect)
{
	//PINC |= 0xff;
	g_millis++;
}

// Resets the g_millis variable back to 0
void resetMillis()
{
	g_lastOVF = g_millis;
	g_millis = 0;
}

int getLastOVF()
{
	return g_lastOVF;
}

// Returns the number of milliseconds since the las call to resetMillis()
int getMillis()
{
	return g_millis;
}

// Initializes a millisecond timer
void millisTimerInit()
{
	TCCR0A |= (1<<WGM01);
	TCCR0B |= (1<<CS01) | (1<<CS00);
	OCR0A = 249;
	TIMSK0 |= (1<<OCIE0A);
}