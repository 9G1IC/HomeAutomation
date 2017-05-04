#ifndef F_CPU 
#define F_CPU 16000000UL
#endif
#define defaultBaudRate F_CPU/16/9600 - 1 
#include <avr/io.h>

typedef struct metaData{
	uint8_t rxSize;
	char * data;
} UART;


UART * uartMetaData;
char * udr0Temp;
char * udr0Data;
volatile  uint8_t* udr0Counter;
volatile  uint8_t* udr0IsReady;
volatile  uint8_t* udr0RxSize;

void uart_tx( char *, uint8_t);
void uart_tx_3( char *, uint8_t);
UART * uart_rx();
void initializeUart(unsigned int baudRate);
void bufferUDR0(char temp,uint8_t *counter,uint8_t *isReady);
