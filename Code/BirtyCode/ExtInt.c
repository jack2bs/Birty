/*
 * ExtInt.c
 *
 * Created: 5/27/2022 10:50:52 AM
 *  Author: Jack2bs
 */ 

// Other includes
#include <avr/io.h>
#include <avr/interrupt.h>

// Custom includes
#include "VL6180XDist.h"
#include "Millis.h"

// Holds whether there was an interrupt from one of the distance sensors
static int g_VL6180XCheckFlag = 0;

// Debugging vars
extern int g_diff;
int g_lastCount = 0;

// Pin E6 External interrupt
ISR(INT6_vect)
{
	g_diff = getMillis() - g_lastCount;		// DEBUGGING Time between Distance sensor interrupts
	g_lastCount = getMillis();				// DEBUGGING Time between Distance sensor interrupts
	g_VL6180XCheckFlag = 1;					// Set the flag to check the distance sensors
}

void setCheckVL6180xFlag()
{
	g_VL6180XCheckFlag = 1;
}

// Resets the VL6180x Check flag value
void resetCheckVL6180xFlag()
{
	g_VL6180XCheckFlag = 0;
}

// Returns the VL6180x check flag value
int getCheckVL6180XFlag()
{
	return g_VL6180XCheckFlag;
}

// Initialize the external interrupt
void extIntInit()
{
	// Set INT6 to Falling edge
	EICRB |= (1 << ISC61);
	
	// Enable INT6
	//EIMSK |= (1 << INT6);
}
