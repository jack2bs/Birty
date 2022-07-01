/*
 * DCMotor.c
 *
 * Created: 5/28/2022 3:58:16 PM
 *  Author: Jack2bs
 */ 

// Other Includes
#include <avr/io.h>
#include <stdint.h>

// Custom Includes
#include "DCMotor.h"
#include "MotorDefs.h"



// Get the left motor's direction
int getLeftMotorDir()
{
	if (PINB & (1<<PINB4))											// If PINB4 is high (const pin is high)
	{
		return DIR_BACKWARD;										// Then we are rotating backwards
	}
	else															// Otherwise
	{
		return DIR_FORWARD;											// We are rotating forwards
	}
}

// Get the right motor's direction
int getRightMotorDir()
{
	if (PINB & (1<<PINB5))											// If PINB5 is high (const pin is high)
	{
		return DIR_BACKWARD;										// Then we are rotating backwards
	}
	else															// Otherwise
	{
		return DIR_FORWARD;											// We are rotating forwards
	}
}

// Get one of the motor's direction
int getMotorDir(uint8_t rightOrLeft)
{
	if (rightOrLeft == RIGHT_MOTOR_PASS)							// If the right motor's dir was asked for
	{
		return getRightMotorDir();									// Return the right motor's direction
	}
	if (rightOrLeft == LEFT_MOTOR_PASS)								// If the left motor's dir was asked for
	{
		return getLeftMotorDir();									// Return the left motor's direction
	}
	return -1;														// If an invalid side was asked for return -1
}



// Set the left motor's direction
void setLeftMotorDir(uint8_t dir)
{
	// If the direction is forward
	if (dir == DIR_FORWARD)
	{
		PORTB &= ~(1<<PORTB4);										// Set the constant pin as GND
		TCCR1A &= ~(1<<COM1B0);										// Set PWM pin on top, clear on Comp
	}
	//Otherwise
	else
	{
		PORTB |= (1<<PORTB4);										// Set the constant pin as PWR
		TCCR1A |= (1<<COM1B0);										// Clear PWM pin on top, set on Comp
	}
}

// Set the right motor's direction
void setRightMotorDir(uint8_t dir)
{
	// If the direction is forward
	if (dir == DIR_FORWARD)
	{
		PORTB &= ~(1<<PORTB5);										// Set the constant pin as GND
		TCCR1A &= ~(1<<COM1C0);										// Set PWM pin on top, clear on Comp
	}
	else
	{
		PORTB |= (1<<PORTB5);										// Set the constant pin as PWR
		TCCR1A |= (1<<COM1C0);										// Clear PWM pin on top, set on Comp
	}
}

// Set either or Both of the motors direction
void setMotorDir(uint8_t rightOrLeft, uint8_t dir)
{
	if (rightOrLeft & RIGHT_MOTOR_PASS)								// If Right motor or Both motors
	{
		setRightMotorDir(dir);										// Set the Right Motor Dir
	}
	if (rightOrLeft & LEFT_MOTOR_PASS)								// If Left motor or Both motors
	{
		setLeftMotorDir(dir);										// Set the Left Motor Dir
	}
}





// Gets the left motor's PWR as a percentage/duty cycle
int getLeftMotorPWR()
{
	return OCR1B / 100;
}

// Gets the right motor's PWR as a percentage/duty cycle
int getRightMotorPWR()
{
	return OCR1C / 100;
}

// Gets the motor PWR as a percentage/duty cycle
int getMotorPWR(uint8_t rightOrLeft)
{
	if (rightOrLeft == RIGHT_MOTOR_PASS)							// Return the right motor's power
	{
		return OCR1C / 100;
	}
	if (rightOrLeft == LEFT_MOTOR_PASS)								// Return the left motor's power
	{
		return OCR1B / 100;
	}
	return -1;														// If invalid motor choice, return -1
}



// Set the left motors power as a percentage/duty cycle
void setLeftMotorPWR(uint8_t percent)
{
	OCR1B = ((uint16_t) percent) * 100;								// Set OCR1B based on percentage with 10000 as 100% and 0 as 0%
}

// Set the right motors power as a percentage/duty cycle
void setRightMotorPWR(uint8_t percent)
{
	OCR1C = ((uint16_t) percent) * 100;								// Set OCR1C based on percentage with 10000 as 100% and 0 as 0%
}

// Set either or both of the motors' power as a percentage/duty cycle
void setMotorPWR(uint8_t rightOrLeft, uint8_t percent)
{
	if (rightOrLeft & RIGHT_MOTOR_PASS)								// If Right motor or Both motors
	{
		setRightMotorPWR(percent);									// Set the Right Motor PWR
	}
	if (rightOrLeft & LEFT_MOTOR_PASS)								// If Left motor or Both motors
	{
		setLeftMotorPWR(percent);									// Set the Left Motor PWR
	}
}



// Initializes DC Motor Timer
void motorTimerInit()
{
	TCCR1A |= (1<<COM1B1) | (1<<COM1C1) | (1<<WGM11) | (1<<WGM10);	// COM Bits: set on top, clear on Comp;  WGM Bits set fast PWM
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS30);					// CS30 enables timer without prescaler; WGM Bits set fast PWM
	OCR1A = 10000;													// Sets TOP as 10000
}

// Initialize the DC Motors
void dcMotorInit(void)
{
	DDRB |= (1<<DDB4) | (1<<DDB5) | (1<<DDB6) | (1<<DDB7);			// Set the motor driving pins as outputs
	motorTimerInit();												// Initialize the motor timers
	setMotorDir(BOTH_MOTORS_PASS, DIR_FORWARD);						// Set both motors to driving forwards
	setMotorPWR(BOTH_MOTORS_PASS, 85);								// Start both motors at 85% power (could remove this line)
}