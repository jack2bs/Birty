/*
 * Encoder.h
 *
 * Created: 5/28/2022 11:22:54 PM
 *  Author: Jack2bs
 */ 


#ifndef ENCODER_H_
#define ENCODER_H_

void resetLeftDist();			// Reset the encoder variable corresponding to the left motor
void resetRightDist();			// Reset the encoder variable corresponding to the right motor
void resetDists();				// Reset both encoder distances

int getLeftDistCM();				// Get the left motor's distance
int getRightDistCM();				// Get the right motor's distance
int getDistCM(int rightLeft);		// Get one of the motor's distances

long getLeftDistAccurate();
long getRightDistAccurate();
long getDistAccurate(int rightLeft);

void encoderInit(void);			// Initialize the encoders

#endif /* ENCODER_H_ */