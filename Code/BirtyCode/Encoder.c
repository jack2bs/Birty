/*
 * Encoder.c
 *
 * Created: 5/28/2022 11:04:51 PM
 *  Author: Jack2bs
 */ 

// Other includes
#include <avr/io.h>
#include <avr/interrupt.h>

// Custom includes
#include "DCMotor.h"
#include "MotorDefs.h"

// encoder variables that store the distance traveled (in meters to the -7 :( )
static long g_encoders[2];

/*

THE FORWARD/BACK AND RIGHT/LEFT MAY BE WRONG

*/

// Pin Change interrupt: Should happen when Encoders detect
ISR(PCINT0_vect)
{
	static uint8_t lastState = 0;												// Stores PORTB's state from the last interrupt
	static const int perEncTick = 1346;	/* 2 * pi * 3cm / (14 counts per rev * 50 the gear ratio) */
	
	
	for(int i = 0; i < 4; i++)													// For each encoder pin
	{
		if ((lastState & (1<<i)) != (PINB & (1<<i)))							// If the last state is different from this state
		{
			if (i < 2)															// If i < 2 then it is one of the left motor's encoders
			{
				if (getLeftMotorDir() == DIR_FORWARD)							// If direction is forward then we want to increment the encoder vals
				{
					g_encoders[LEFT_MOTOR_INDEX] +=  perEncTick;
				}
				else
				{
					g_encoders[LEFT_MOTOR_INDEX] -= perEncTick;
				}
			}
			else
			{
				if (getRightMotorDir() == DIR_FORWARD)
				{
					g_encoders[RIGHT_MOTOR_INDEX] += perEncTick;
				}
				else
				{
					g_encoders[RIGHT_MOTOR_INDEX] -= perEncTick;
				}
			}
		}
	}
	lastState = (PINB & 0b1111);												// Update lastState
}

// Reset the encoder variable corresponding to the left motor
void resetLeftDist()
{
	g_encoders[LEFT_MOTOR_INDEX] = 0;											// Resets the Left motor's encoder value to 0
}

// Reset the encoder variable corresponding to the right motor
void resetRightDist()
{
	g_encoders[RIGHT_MOTOR_INDEX] = 0;											// Resets the Right motor's encoder value to 0
}

// Reset both encoder distances
void resetDists()
{
	resetLeftDist();															// Reset the left encoder's variable
	resetRightDist();															// Reset the right encoder's variable
}

// Get the left motor's distance
int getLeftDistCM()
{
	return g_encoders[LEFT_MOTOR_INDEX] / 100000;
}

// Get the right motor's distance
int getRightDistCM()
{
	return g_encoders[RIGHT_MOTOR_INDEX] / 100000;
}

// Get one of the motor's distances
int getDistCM(int rightLeft)
{
	if (rightLeft == RIGHT_MOTOR_PASS)											// If the choice is Right
	{
		return getRightDistCM();													// Return the right dist
	}
	if (rightLeft == LEFT_MOTOR_PASS)											// If the choice is Left
	{
		return getLeftDistCM();													// Return the left dist
	}
	return -1;																	// If the choice is invalid return -1
}

long getLeftDistAccurate()
{
	return g_encoders[LEFT_MOTOR_INDEX];
}

long getRightDistAccurate()
{
	return g_encoders[RIGHT_MOTOR_INDEX];
}

// Get one of the motor's distances
long getDistAccurate(int rightLeft)
{
	if (rightLeft == RIGHT_MOTOR_PASS)											// If the choice is Right
	{
		return getRightDistAccurate();													// Return the right dist
	}
	if (rightLeft == LEFT_MOTOR_PASS)											// If the choice is Left
	{
		return getLeftDistAccurate();													// Return the left dist
	}
	return -1;																	// If the choice is invalid return -1
}


// Initialize the encoders
void encoderInit(void)
{
	DDRB &= ~(1<<DDB0) | ~(1<<DDB1) | ~(1<<DDB2) | ~(1<<DDB3);					// Set the encoder pins to inputs
	DDRD |= (1<<DDD4) | (1<<DDD5) | (1<<DDD6) | (1<<DDD7);						// Set the encoder power pins to outputs
	PORTD |= (1<<PORTD7) | (1<<PORTD6);											// Set pins d6 and d7 high,
	PORTD &= ~(1<<PORTD4) & ~(1<<PORTD5);										// And pins d4 and d5 low
	PCICR |= (1<<PCIE0);														// Enable the Pin Change interrupt
	PCMSK0 |= (1<<PCINT0) | (1<<PCINT1) | (1<<PCINT2) | (1<<PCINT3);			// Enable the Pin Change interrupts on the encoder pins
}