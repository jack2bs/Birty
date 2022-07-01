/*
 * I2CInstruction.h
 *
 * Created: 5/26/2022 9:21:31 PM
 *  Author: Jack2bs
 */ 


#ifndef I2CINSTRUCTION_H_
#define I2CINSTRUCTION_H_

#define BUFFER_SIZE 127		// Size of the instruction buffer

#define I2C_WRITE	0
#define I2C_READ	1

// Custom I2C instruction Struct
struct I2CInstruction
{
	int dev_index;
	int readWrite;
	uint8_t* data;
	int length;
};

int getr_ptr();													// Returns the location in the buffer that will be read next/is being read
int I2CInstrGetCurrentSize();									// Returns the current number of instructions in the buffer
int I2CInstrGetFull();											// Returns true if the Buffer is full
int I2CInstrGetEmpty();											// Returns true if the Buffer is empty
int moveToNextInstruction();									// Moves to the next instruction
struct I2CInstruction * getCurrentInstruction();				// Returns a pointer to the current instruction
int addInstruction(int d_ind, int rw, uint8_t* dat, int leng);	// Adds an instruction at w_ptr


#endif /* I2CINSTRUCTION_H_ */