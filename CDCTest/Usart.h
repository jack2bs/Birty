/*
 * Usart.h
 *
 * Created: 6/25/2022 11:04:07 AM
 *  Author: Jack2bs
 */ 


#ifndef USART_H_
#define USART_H_

int addByteToUsartWriteBuffer(uint8_t data);
void addStringToUsartWriteBuffer(uint8_t * str);
int getUsartWriteBufferSize();

void usartInit();
void usartTask();

#endif /* USART_H_ */