/*
 * Stepper.c
 *
 * Created: 5/29/2022 11:10:15 AM
 *  Author: Jack2bs
 */ 

// Other includes
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Custom includes
#include "Stepper.h"
#include "MotorDefs.h"

// This variable stores the 1% power TOP for the stepper CTC
static const int minPower = 18800;

// This stores the direction of the two stepper motors
static uint8_t g_direction[2] = {DIR_FORWARD, DIR_FORWARD};

// This occurs at TOP for the left motor's CTC
ISR(TIMER1_COMPA_vect)
{
	fullStepLeft(g_direction[LEFT_MOTOR_INDEX]);
}

// This occurs at TOP for the right motor's CTC
ISR(TIMER3_COMPA_vect)
{
	fullStepRight(g_direction[RIGHT_MOTOR_INDEX]);
}



// This sets the left stepper's direction
void setStepperMotorDirLeft(uint8_t dir)
{
	g_direction[LEFT_MOTOR_INDEX] = dir;
}

// This sets the right stepper's direction
void setStepperMotorDirRight(uint8_t dir)
{
	g_direction[RIGHT_MOTOR_INDEX] = dir;
}

// This sets either or both motor's direction
void setStepperMotorDir(uint8_t rightOrLeft, uint8_t dir)
{
	if (rightOrLeft & RIGHT_MOTOR_PASS)
	{
		g_direction[RIGHT_MOTOR_INDEX] = dir;
	}
	if (rightOrLeft & LEFT_MOTOR_PASS)
	{
		g_direction[LEFT_MOTOR_INDEX] = dir;
	}
}



// This returns the direction of the Left motor
uint8_t getStepperMotorDirLeft()
{
	return g_direction[LEFT_MOTOR_INDEX];
}

// This returns the direction of the Right motor
uint8_t getStepperMotorDirRight()
{
	return g_direction[RIGHT_MOTOR_INDEX];
}

// This returns the direction of either stepper
uint8_t getStepperMotorDir(uint8_t rightOrLeft)
{
	return g_direction[rightOrLeft - 1];
}



// This returns the power of the left stepper as a percentage
int getStepperPowerLeft()
{
	int temp = OCR1A;
	return (minPower / temp);
}

// This returns the power of the right stepper as a percentage
int getStepperPowerRight()
{
	int temp = OCR3A;
	return (minPower / temp);
}

// This returns the power of one of the motors
int getStepperPower(int rightOrLeft)
{
	// If the Right motor was selected
	if (rightOrLeft == RIGHT_MOTOR_PASS)
	{
		return getStepperPowerRight();
	}
	// Else if the left motor was selected
	else if (rightOrLeft == LEFT_MOTOR_PASS)
	{
		return getStepperPowerLeft();
	}
	// Else return -1
	return -1;
}



// The sets the left stepper motor's power
void setStepperPowerLeft(int percent)
{
	// If it's 0 percent
	if (!percent)
	{
		// Disable the CTC
		TCCR1B = 0x00;
		return;
	}
	
	// Reinitialize the left stepper motor's timer (in case we are recovering from 0%)
	stepperTimerLeftInit();
	
	// Divide the minimum power by the percent to get our TOP value
	OCR1A = (uint16_t)(minPower / percent);
}

// The sets the right stepper motor's power
void setStepperPowerRight(int percent)
{
	// If it's 0 percent
	if (!percent)
	{
		// Disable the CTC
		TCCR3B = 0x00;
		return;
	}
	
	// Reinitialize the left stepper motor's timer (in case we are recovering from 0%)
	stepperTimerRightInit();
	
	// Divide the minimum power by the percent to get our TOP value
	OCR3A = (uint16_t)(minPower / percent);
}

// This sets either or both stepper's power
void setStepperPower(int rightOrLeft, int percent)
{
	// If Left or Both is passed
	if (rightOrLeft & LEFT_MOTOR_PASS)
	{
		setStepperPowerLeft(percent);		// Set the left motor's power
	}
	// If Right or Both is passed
	if (rightOrLeft & RIGHT_MOTOR_PASS)
	{
		setStepperPowerRight(percent);		// Set the right motor's power
	}
}



// Have the left motor take a full step
void fullStepLeft(uint8_t forwardBackward)
{
	// This stores the next state of the Left stepper
	static int8_t l_state = 0;
	
	/*
	States:
	0: B4, D4
	1: B6, D4
	2: B6, D6
	3: B4, D6
	*/
	
	// Switch: the state of the left stepper
	switch (l_state)
	{
	// Move the motor to l_state
	case 0:
		PORTD |= (1<<PORTD4);
		PORTB |= (1<<PORTB4);
		PORTD &= ~(1<<PORTD6);
		PORTB &= ~(1<<PORTB6);
		break;
	case 1:
		PORTD |= (1<<PORTD4);
		PORTB |= (1<<PORTB6);
		PORTD &= ~(1<<PORTD6);
		PORTB &= ~(1<<PORTB4);
		break;
	case 2:
		PORTD |= (1<<PORTD6);
		PORTB |= (1<<PORTB6);
		PORTD &= ~(1<<PORTD4);
		PORTB &= ~(1<<PORTB4);
		break;
	case 3:
		PORTD |= (1<<PORTD6);
		PORTB |= (1<<PORTB4);
		PORTD &= ~(1<<PORTD4);
		PORTB &= ~(1<<PORTB6);
		break;
	default:
		l_state = 0;	
		return;
	}
	
	// If the motor is supposed to be moving forward
	if (forwardBackward == DIR_FORWARD)
	{
		l_state++;	// Increment the l_state
	}
	// Otherwise
	else
	{
		l_state--;	// Decrement the l_state
	}
	
	// If the motor's state was incremented too high
	if (l_state > 3)
	{
		// Reset it to 0
		l_state = 0;
	}
	// Otherwise if it is decremented too low
	else if (l_state < 0)
	{
		// Reset it to 3
		l_state = 3;
	}
}

// Have the right motor take a full step
void fullStepRight(uint8_t forwardBackward)
{
	// This stores the next state of the Right stepper
	static int8_t r_state = 0;
	
	/*
	States:
	0: B5, D5
	1: B7, D5
	2: B7, D7
	3: B5, D7
	*/
	
	// Switch: the state of the right stepper
	switch (r_state)
	{
	// Move the motor to r_state
	case 0:
		PORTD |= (1<<PORTD5);
		PORTB |= (1<<PORTB5);
		PORTD &= ~(1<<PORTD7);
		PORTB &= ~(1<<PORTB7);
		break;
	case 1:
		PORTD |= (1<<PORTD5);
		PORTB |= (1<<PORTB7);
		PORTD &= ~(1<<PORTD7);
		PORTB &= ~(1<<PORTB5);
		break;	
	case 2:
		PORTD |= (1<<PORTD7);
		PORTB |= (1<<PORTB7);
		PORTD &= ~(1<<PORTD5);
		PORTB &= ~(1<<PORTB5);
		break;
	case 3:
		PORTD |= (1<<PORTD7);
		PORTB |= (1<<PORTB5);
		PORTD &= ~(1<<PORTD5);
		PORTB &= ~(1<<PORTB7);
		break;
	default:
		r_state = 0;	
		return;
	}
	
	// If the motor is supposed to be moving forward
	if (forwardBackward == DIR_FORWARD)
	{
		r_state++;	// Increment the r_state
	}
	// Otherwise
	else
	{
		r_state--;	// Decrement the r_state
	}
	
	// If the motor's state was incremented too high
	if (r_state > 3)
	{
		// Reset it to 0
		r_state = 0;
	}
	// Otherwise if it is decremented too low
	else if (r_state < 0)
	{
		// Reset it to 3
		r_state = 3;
	}
}



// Initialize the timer of the left stepper motor
inline void stepperTimerLeftInit()
{
	// Set the timer to CTC mode only interrupting at TOP
	TCCR1B |= (1<<WGM12) | (1<<CS10) | (1<<CS11);
	TIMSK1 |= (1<<OCIE1A);
}

// Initialize the timer of the right stepper motor
inline void stepperTimerRightInit()
{
	// Set the timer to CTC mode only interrupting at TOP
	TCCR3B |= (1<<WGM32) | (1<<CS30) | (1<<CS31);
	TIMSK3 |= (1<<OCIE3A);
}

// Initialize both motor's timers
void stepperTimerInit()
{
	stepperTimerLeftInit();
	stepperTimerRightInit();
}



// Initialize the stepper motors
void stepperInit()
{
	// Set all stepper motor controlling pins to outputs
	DDRB |= (1<<DDB4) | (1<<DDB5) | (1<<DDB6) | (1<<DDB7);
	DDRD |= (1<<DDD4) | (1<<DDD5) | (1<<DDD6) | (1<<DDD7);
	
	// Initialize the stepper timers
	stepperTimerInit();
	
	// This could be removed
	setStepperPower(BOTH_MOTORS_PASS, 0);
}