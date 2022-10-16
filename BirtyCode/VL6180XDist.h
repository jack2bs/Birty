/*
 * VL6180XDist.h
 *
 * Created: 5/27/2022 12:25:15 AM
 *  Author: Jack2bs
 */ 


#ifndef VL6180XDIST_H_
#define VL6180XDIST_H_

#define FRONT_LEFT_INDEX			0
#define	FRONT_RIGHT_INDEX			1
#define LEFT_FRONT_INDEX			2
#define LEFT_BACK_INDEX				3
#define RIGHT_FRONT_INDEX			4
#define RIGHT_BACK_INDEX			5
#define BACK_RIGHT_INDEX			6
#define BACK_LEFT_INDEX				7

#define NUM_PRIVATE_REGS	30						// Stores the number of private regs I have to change
#define NUM_PUBLIC_REGS		12						// Stores the number of public regs I want to change

void VL6180XAddReadInstruction(int index);			// This adds the instructions for a VL6180X Read
void VL6180XInit(int index);						// Initializes a VL6180x sensor
void VL6180XTask();									// Handles VL6180x periodic things
void VL6180XRawToString(uint8_t raw, char* str);	// Turns a 3 digit (Base 10) number to a string
void VL6180XCheckForFailures();
int getDistSensorDist(int index);

#endif /* VL6180XDIST_H_ */