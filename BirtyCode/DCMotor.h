/*
 * DCMotor.h
 *
 * Created: 5/28/2022 4:09:29 PM
 *  Author: Jack2bs
 */ 


#ifndef DCMOTOR_H_
#define DCMOTOR_H_

#include <stdint.h>

int getLeftMotorDir();										// Get the left motor's direction
int getRightMotorDir();										// Get the right motor's direction
int getMotorDir(uint8_t rightOrLeft);						// Get one of the motor's direction

void setLeftMotorDir(uint8_t dir);							// Set the left motor's direction
void setRightMotorDir(uint8_t dir);							// Set the right motor's direction
void setMotorDir(uint8_t rightOrLeft, uint8_t dir);			// Set either or Both of the motors direction

int getLeftMotorPWR();										// Gets the left motor's PWR as a percentage/duty cycle
int getRightMotorPWR();										// Gets the right motor's PWR as a percentage/duty cycle
int getMotorPWR(uint8_t rightOrLeft);						// Gets the motor PWR as a percentage/duty cycle

void setLeftMotorPWR(uint8_t percent);						// Set the left motors power as a percentage/duty cycle
void setRightMotorPWR(uint8_t percent);						// Set the right motors power as a percentage/duty cycle
void setMotorPWR(uint8_t rightOrLeft, uint8_t percent);		// Set either or both of the motors' power as a percentage/duty cycle

void motorTimerInit();										// Initializes DC Motor Timer
void dcMotorInit(void);										// Initialize the DC Motors

#endif /* DCMOTOR_H_ */