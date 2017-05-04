#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "uart.h"

#define defaultBaudRate F_CPU/16/9600 - 1 
#define USART_INTERRUPT

void sendGSMData( volatile char * _data,int size){
	uart_tx_3(_data,size);
		 while(!(UCSR3A & _BV(UDRE3)));
		 UDR3 = '\n';
		 while(!(UCSR3A & _BV(UDRE3)));
		 UDR3 = '\r';
	_delay_ms(105);
}

void testGSM(char * _data,uint8_t size,char*tokens[]){
	uint8_t i=1;
			uart_tx("\n\r============\n\r",16);
			uart_tx(_data,size);
			uart_tx("\n\r____________\n\r",16);
			uart_tx(tokens[i],strlen(tokens[i]));
			uart_tx("\n\r^^^^^^^^^^^^\n\r",16);
		}

int main(){
	cli();
	char * info;
	char *gsmTokens[6];
	char * buffer;
	gsmBuffer = (char *)malloc(sizeof(char));
	udr0Data = (char *)malloc(sizeof(char));

	initializeUart(defaultBaudRate);

	getGSMData("at+cmgf=1",9);
	while(1){
		bufferUD0();
		getGSMData("at+cmgr=14,1",12);
		if(udr0Isready==1){
			if(udr0RxSize > 0){
				getGSMData(udr0Data,udr0RxSize);
				udr0RxSize=0;
			} 
			if(gsmBufferSize > 0){ 
				int i=0;
				char c;
				buffer=gsmBuffer;
				extractGSMTokens(buffer,gsmTokens,"#");
				testGSM(buffer,gsmBufferSize,gsmTokens);

				free(gsmBuffer);
				gsmBuffer = (char *)malloc(sizeof(char));
				gsmBufferSize=0;
			}
		}
	}
}
#endif
