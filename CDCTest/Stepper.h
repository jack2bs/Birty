/*
 * Stepper.h
 *
 * Created: 5/29/2022 11:16:29 AM
 *  Author: Jack2bs
 */ 


#ifndef STEPPER_H_
#define STEPPER_H_

void stepperTimerLeftInit();								// Initialize the timer of the left stepper motor
void stepperTimerRightInit();								// Initialize the timer of the right stepper motor
void stepperTimerInit();									// Initialize both motor's timers
void stepperInit();											// Initialize the stepper motors

void setStepperMotorDir(uint8_t rightOrLeft, uint8_t dir);	// This sets either or both motors' direction
void setStepperMotorDirLeft(uint8_t dir);					// This sets the left stepper's direction
void setStepperMotorDirRight(uint8_t dir);					// This sets the right stepper's direction

uint8_t getStepperMotorDir(uint8_t rightOrLeft);			// This returns the direction of either stepper
uint8_t getStepperMotorDirLeft();							// This returns the direction of the Left motor
uint8_t getStepperMotorDirRight();							// This returns the direction of the Right motor

void fullStepLeft(uint8_t forwardBackward);					// Have the left motor take a full step
void fullStepRight(uint8_t forwardBackward);				// Have the right motor take a full step

void setStepperPower(int rightOrLeft, int percent);			// This sets either or both stepper's power
void setStepperPowerLeft(int percent);						// The sets the left stepper motor's power
void setStepperPowerRight(int percent);						// The sets the right stepper motor's power

int getStepperPower(int rightOrLeft);						// This returns the power of one of the motors
int getStepperPowerLeft();									// This returns the power of the left stepper as a percentage
int getStepperPowerRight();									// This returns the power of the right stepper as a percentage


#endif /* STEPPER_H_ */