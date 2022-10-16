/*
 * ShiftReg.c
 *
 * Created: 6/18/2022 5:16:56 PM
 *  Author: Jack2bs
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "ShiftReg.h"

void shiftInBit(uint8_t in)
{
	if (in)
	{
		PORTF &= ~(1<<PORTF0);
	}
	else
	{
		PORTF |= (1<<PORTF0);
	}
	_delay_us(5);
	PINF |= (1<<PINF1);
	_delay_us(5);
	PINF |= (1<<PINF1);
	_delay_us(5);

}

void shiftRegInit()
{
	DDRF |= (1<<DDF1) | (1<<DDF0);
	PORTF |= (1<<PORTF0);
	PORTF &= ~(1<<PORTF1);
	for (int i = 0; i < 20; i++)
	{
		PORTF |= (1<<PORTF1);
		_delay_us(5);
		PORTF &= ~(1<<PORTF1);
		_delay_us(5);
	}
}