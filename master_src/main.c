#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
//Local include
#include "hardware.h"
#include "messanger.h"
#include "loadController.h"
#include "smartMeterController.h"
#include "../../snippet/io.h"
#include "uart.h"
#include "gsm.h"
#include <ctype.h>
// Scenario 2:
//furo cooks with an electric cooker which consumes tonnes of power. the smart meter measures the consumed power, and converts it to CONSUMED UNITS. The CONSUMED UNITS are deduced from the RECHARED UNITS. This process goes on until the CONSUMED UNITS reaches the CRITICAL UNITS, at which point, the meter alerts Furo, by sending 3 LONG BEEPS and displaying "LOW CREDIT" on the LCD. However, Furo does not heed the warning, and he leaves the cooker on. Finally, the CONSUMED UNIT is equal to the RECHARED UNIT, and the SMART trips OFF the relay of the load.

#define USART_INTERRUPT
#define DEVELOPMENT


volatile uint8_t gsmBufferSize;
volatile char * gsmBuffer;
volatile char * gsmTokens[3];
uint8_t gsmPingDuration=3;
volatile unsigned int gsmStopPingTime=0;
volatile uint8_t testTime=0;
void messageMonitor(MESSAGE *,ELECTRICPOWER *);


ISR(TIMER1_COMPB_vect){
time->elapsedTime < 0?(time->elapsedTime=0):(time->elapsedTime ++);
time->elapsedTime > 3600?(time->elapsedTime=0,gsmStopPingTime = gsmPingDuration,testTime = gsmPingDuration)  :(time->elapsedTime ++);
if(time->seconds++ >= 59)
{
	time->minutes++;
	time->seconds^=time->seconds;
}
else
if(time->minutes >= 59)
{
	time->hours++;
	time->minutes^=time->minutes;
}
else
if(time->hours >= 23) time->hours ^=time->hours;
pingGSM(Message,gsmPingDuration);
}

ISR(ADC_vect){
	power->mainsCurrent += (uint8_t) ADCL;
	measurePower(power);
}

#ifdef USART_INTERRUPT
uint8_t terminalRxSize;
uint8_t isReady=0;
char temp;

ISR(USART0_RX_vect){
		temp = UDR0;
		if(temp == '\r'){
		 	while(!(UCSR0A & _BV(UDRE0)));
			UDR0 = '\n';
		 	while(!(UCSR0A & _BV(UDRE0)));
			UDR0 = '\r';
		 	while(!(UCSR0A & _BV(UDRE0)));
			UDR0 = '>';
		        terminalRxSize=0;
			temp=' ';
			isReady=1;
			goto end;
		}
		else isReady = 0;
		while(!(UCSR0A & _BV(UDRE0)));
		UDR0 = (char)temp;
		Message->data[terminalRxSize++] =toupper(temp);
		uartMetaData->rxSize = terminalRxSize;
		Message->displayMessageLength = terminalRxSize;
end:{
		*displaySwitch=1;
    }
}

ISR(USART3_RX_vect){
		gsmBuffer[gsmBufferSize++] = toupper(UDR3) ;
}
#endif










#ifdef DEVELOPMENT
void bufferUD0(){
	if(temp =='\r')
		{
		 	while(!(UCSR0A & _BV(UDRE0)));
			UDR0 = '\n';
		 	while(!(UCSR0A & _BV(UDRE0)));
			UDR0 = '\r';
		 	while(!(UCSR0A & _BV(UDRE0)));
			UDR0 = '>';
		        terminalRxSize=0;
			temp=' ';
			isReady=1;
		}
}

void test(uint8_t function){
	switch(function){
	case 0:
		//Ensure Message->displayMessageLength is counted correctly
	messageMonitor(Message,power);
	showGSMMessage(Message);
		break;
	case 1:
	getTime();
		break;	

	case 2:
	messageMonitor(Message,power);
	displayMessage(messageBank(PERIOD),10);
	write(messageBank(GSMDATA),8);
	write(messageBank( COLUMN ),1);
	write(Message->data,Message->displayMessageLength);
	write(messageBank( COLUMN ),1);
	displayNumber(Message->displayMessageLength);

	write(messageBank(COMMAND),7);
	write(messageBank(COLUMN),1);
	write(Message->command,Message->commandLength);
	write(messageBank( COLUMN ),1);
	displayNumber(Message->commandLength);

	write(messageBank(PARAMETER),9);
	write(messageBank( COLUMN ),1);
	write(Message->commandParameter,Message->commandParameterLength);
	write(messageBank( COLUMN ),1);
	displayNumber(Message->commandParameterLength);
	break;
	case 3:
		messageMonitor(Message,power);
		loadMonitor(Message);
		meterMonitor(power);
	break;
	case 4: 
	gsmResponseManager(Message->data,Message->displayMessageLength,gsmBuffer,gsmTokens);
	break;
	}
}
#endif

void loadMessageBuffer(MESSAGE * message,char * source,uint8_t size){
				uint8_t i=0;
				for(;i<size;i++)
				message->data[i]=source[i];
				message->displayMessageLength=i;
}

uint8_t loaded=0;
MESSAGE * gsmMessage;
void pingGSM(MESSAGE * message, uint8_t period){

		if(time->elapsedTime > gsmStopPingTime ){
		gsmStopPingTime=time->elapsedTime+period;
		loadMessageBuffer(gsmMessage,messageBank(ATCOMMANDLIST),20);
		loaded=1;
		}

}

void gsmResponseManager(MESSAGE * gsmToTerminalMessage,volatile char * gsmResponse,volatile char *bufferForGSMTokens[]){

volatile char * gsmTemporaryBuffer;

gsmTemporaryBuffer=gsmResponse;

if(strchr(gsmTemporaryBuffer,'#')){
extractGSMTokens(gsmTemporaryBuffer,bufferForGSMTokens,"#");
loadMessageBuffer(gsmToTerminalMessage,bufferForGSMTokens[1],strlen(bufferForGSMTokens[1]));
testGSM(gsmTemporaryBuffer,gsmBufferSize,bufferForGSMTokens);
	}
else
if(strstr(gsmTemporaryBuffer,"CMTI")){
displayMessage(gsmResponse,gsmBufferSize);
extractGSMTokens(gsmTemporaryBuffer,bufferForGSMTokens,",");
char * newMessageIndex;
newMessageIndex = strdup(messageBank(ATCOMMANDREAD));
strncat(newMessageIndex,bufferForGSMTokens[1],strlen(bufferForGSMTokens[1]));
strncat(newMessageIndex,messageBank(COMMAR),1);
strncat(newMessageIndex,"1",1);

sendGSMData(newMessageIndex,12);
newMessageIndex=NULL;
	}
else
if(strstr(gsmTemporaryBuffer,"RING")){
displayMessage(gsmResponse,gsmBufferSize);
}
else
displayMessage(gsmResponse,gsmBufferSize);

free(gsmResponse);
free(gsmTemporaryBuffer);
gsmResponse = (volatile char *)malloc(sizeof(volatile char));
gsmTemporaryBuffer = (volatile char *)malloc(sizeof(volatile char));
gsmBufferSize=0;
}

int main(){
cli();
gsmBufferSize=0;
terminalRxSize=0;
gsmBuffer=(volatile char*)malloc(sizeof(volatile char));
gsmMessage=(MESSAGE *)malloc(sizeof(MESSAGE));
power->rechargedUnits=100;

	initializeHardware();
	initializeGSM();
	initializeMeter();
	*displaySwitch=1;
	firstTime();
sei();	
//Needed to display GSM status
	sendGSMData(messageBank(ATTEXTMODE),9);
//#define DEVELOPMENT
#undef DEVELOPMENT
#ifdef DEVELOPMENT
	sendGSMData("ate1",4);
#else
	sendGSMData(messageBank(ATECHOOFF),4);
#endif

while(1){
		//bufferUD0();
		if(loaded){
		sendGSMData(gsmMessage->data,gsmMessage->displayMessageLength);
		loaded=0;
		}
		if(strstr(gsmBuffer,"AT+")){
			gsmResponseManager(Message,gsmBuffer,gsmTokens);
			goto execute;
		}
		if(isReady > 0){
			goto execute;	
		}
		else goto exit;

execute:{
		*displaySwitch=1;
		messageMonitor(Message,power);
		loadMonitor(Message);
		meterMonitor(power);
		isReady=0;
		}
exit:{
     	}
			*displaySwitch = 0;

	}
}
