/*
 * ExtInt.h
 *
 * Created: 5/27/2022 10:51:01 AM
 *  Author: Jack2bs
 */ 


#ifndef EXTINT_H_
#define EXTINT_H_

void resetCheckVL6180xFlag();	// Resets the VL6180x Check flag value
int getCheckVL6180XFlag();		// Returns the VL6180x check flag value
void extIntInit();				// Initialize the external interrupt
void setCheckVL6180xFlag();

#endif /* EXTINT_H_ */