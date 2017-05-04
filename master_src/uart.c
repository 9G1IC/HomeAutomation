#ifndef F_CPU
#define F_CPU 16000000UL
#endif 

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include "uart.h"

#define USART_INTERRUPT
#define UART3

void initializeUart(unsigned int baudRate){
	UBRR0H = (unsigned char ) (baudRate >> 8);
	UBRR0L = (unsigned char )baudRate;
#ifndef USART_INTERRUPT
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
#else
	UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
#endif
	UCSR0C = _BV(USBS0) | (3<<UCSZ00);

#ifdef UART3
	UBRR3H = (unsigned char ) (baudRate >> 8);
	UBRR3L = (unsigned char )baudRate;
#ifndef USART_INTERRUPT
	UCSR3B = _BV(RXEN3) | _BV(TXEN3);
#else
	UCSR3B = _BV(RXEN3) | _BV(TXEN3) | _BV(RXCIE3);
#endif
	UCSR3C = _BV(USBS3) | (3<<UCSZ30);
#endif

}


void uart_tx( char * data, uint8_t size){
	int counter =0;
	for (;counter<size;counter++){
		 while(!(UCSR0A & _BV(UDRE0)));
		 UDR0 = (char)data[counter];
	}
}

void uart_tx_3( char * _data, uint8_t size){
	uint8_t counter =0;
	for (;counter<size;counter++){
		 while(!(UCSR3A & _BV(UDRE3)));
		 UDR3 = (char)_data[counter];
	}
}

void bufferUDR0(char temp,uint8_t *counter, uint8_t *isReady){
	if(temp =='\r')
		{
		 	while(!(UCSR0A & _BV(UDRE0)));
			UDR0 = '\n';
		 	while(!(UCSR0A & _BV(UDRE0)));
			UDR0 = '\r';
		 	while(!(UCSR0A & _BV(UDRE0)));
			UDR0 = '>';
		        *counter =0;
			temp=' ';
			*isReady=1;
		}
}

#ifndef USART_INTERRUPT
UART * uart_rx(){
	char   temp;
	uint8_t counter=0;
	char * buffer = malloc(sizeof(char));
	while(temp != '\r'){
		while(!(UCSR0A & _BV(RXC0)));
		temp = (char)UDR0;
		strncat(buffer,&temp,1);
		counter++;
		UDR0 = (char)temp;
		if(temp =='\r')
		{
			UDR0 = '\n';
			break;
		}
	}
	
	uartMetaData->rxSize = counter - 1;
	strncpy(uartMetaData->data,buffer,counter);
	free(buffer);
	return uartMetaData;
}
#endif
