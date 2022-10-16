/*
 * Usart.c
 *
 * Created: 6/25/2022 10:34:39 AM
 *  Author: Jack2bs
 */ 

#include <avr/interrupt.h>

#include "PID.h"

/*

	Usart Write Buffer ADT

*/

#define USART_BUFFER_MAX_SIZE 64
static const int g_maxSize = USART_BUFFER_MAX_SIZE;
static int g_r_Ptr = 0;
static int g_wr_Ptr = 0;
static uint8_t g_writeBuffer[USART_BUFFER_MAX_SIZE];
static int g_size = 0;


int addByteToUsartWriteBuffer(uint8_t data)
{
	if (g_size >= g_maxSize)
	{
		return 0;
	}
	g_writeBuffer[g_wr_Ptr] = data;
	g_wr_Ptr++;
	if (g_wr_Ptr >= g_maxSize)
	{
		g_wr_Ptr = 0;
	}
	g_size++;
	return 1;
}

void addStringToUsartWriteBuffer(uint8_t * str)
{
	for (uint8_t * i = str; *i != '\0'; i++)
	{
		addByteToUsartWriteBuffer(*i);
	}
}

int getUsartWriteBufferSize()
{
	return g_size;
}

/*

	END Usart Write Buffer ADT

*/

ISR(USART1_RX_vect)
{
	if (UDR1 == '~')
	{
		stopMoving();
	}
}

ISR(USART1_TX_vect)
{
	if (getUsartWriteBufferSize())
	{
		UDR1 = g_writeBuffer[g_r_Ptr];
		g_r_Ptr++;
		if(g_r_Ptr >= g_maxSize)
		{
			g_r_Ptr = 0;
		}
		g_size--;
	}
}

void usartInit()
{
	// Set the baud rate
	UBRR1 = 8;
	
	// Enable interrupts on receiving and finishing a transmit
	UCSR1B |= (1<<RXCIE1) | (1<<TXCIE1);	// We don't interrupt on empty buffer because we may not always have something we want to write.
	
	// Enable both the receiver and transmitter
	UCSR1B |= (1<<RXEN1) | (1<<TXEN1);
	
}

void usartTask()
{
	if (getUsartWriteBufferSize())
	{
		if (UCSR1A & (1<<UDRE1))
		{
			UDR1 = g_writeBuffer[g_r_Ptr];
			g_r_Ptr++;
			if(g_r_Ptr >= g_maxSize)
			{
				g_r_Ptr = 0;
			}
			g_size--;
		}
	}
}