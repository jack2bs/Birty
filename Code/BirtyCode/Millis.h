/*
 * Millis.h
 *
 * Created: 5/30/2022 5:38:02 PM
 *  Author: Jack2bs
 */ 


#ifndef MILLIS_H_
#define MILLIS_H_

int getLastOVF();
void resetMillis();		// Resets the g_millis variable back to 0
int getMillis();		// Returns the number of milliseconds since the las call to resetMillis()
void millisTimerInit();	// Initializes a millisecond timer


#endif /* MILLIS_H_ */