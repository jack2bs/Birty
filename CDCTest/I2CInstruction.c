/*
 * I2CInstruction.c
 *
 * Created: 5/26/2022 9:21:20 PM
 *  Author: Jack2bs
 */ 

// Other includes
#include <avr/io.h>
#include <avr/interrupt.h>

// Custom includes
#include "I2CInstruction.h"


static struct I2CInstruction g_instructions[BUFFER_SIZE] = {};
	
/* I2C instruction array API */
static int w_ptr = 0;
static int r_ptr = 0;
static int size = 0;

// Returns the location in the buffer that will be read next/is being read
int getr_ptr()
{
	return r_ptr;
}

// Returns the current number of instructions in the buffer
int I2CInstrGetCurrentSize()
{
	return size;
}

// Returns true if the Buffer is full
int I2CInstrGetFull()
{
	// If the size is equal to the buffer size
	if (size == BUFFER_SIZE)
	{
		return 1;
	}
	return 0;
}

// Returns true if the Buffer is empty
int I2CInstrGetEmpty()
{
	// If the size of the buffer is 0
	if (size)
	{
		return 0;
	}
	return 1;
}

// Moves to the next instruction
int moveToNextInstruction()
{
	// Clear this instructions data
	g_instructions[r_ptr].dev_index = 0;	
	g_instructions[r_ptr].readWrite = 0;
	g_instructions[r_ptr].data = 0;
	g_instructions[r_ptr].length = 0;
	
	// If the buffer is empty don't increment the read PTR
	if (I2CInstrGetEmpty())
	{
		return 0;
	}
	
	// Otherwise decrement size and increment the read PTR
	size--;
	r_ptr++;
	if (r_ptr >= BUFFER_SIZE)
	{
		r_ptr = 0;
	}

	return 1;
}

// Returns a pointer to the current instruction
struct I2CInstruction * getCurrentInstruction()
{
	return &(g_instructions[r_ptr]);
}

// Adds an instruction at w_ptr
int addInstruction(int d_ind, int rw, uint8_t* dat, int leng)
{
	// Disable interrupts for this function so as to not have to worry about atomicity
	cli();
	
	// If the buffer is not full
	if(!I2CInstrGetFull())
	{
		// Load in the new instruction's info
		g_instructions[w_ptr].dev_index = d_ind;
		g_instructions[w_ptr].readWrite = rw;
		g_instructions[w_ptr].data = dat;
		g_instructions[w_ptr].length = leng;
		
		// Increment w_ptr and size
		size++;
		w_ptr++;
		if (w_ptr >= BUFFER_SIZE)
		{
			w_ptr = 0;
		}
		sei();
		return 1;
	}
	sei();
	return 0;
}

/* End I2C instruction array API */