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
#include <ctype.h>
#include "gsm.h"
// Scenario 2:
//furo cooks with an electric cooker which consumes tonnes of power. the smart meter measures the consumed power, and converts it to CONSUMED UNITS. The CONSUMED UNITS are deduced from the RECHARED UNITS. This process goes on until the CONSUMED UNITS reaches the CRITICAL UNITS, at which point, the meter alerts Furo, by sending 3 LONG BEEPS and displaying "LOW CREDIT" on the LCD. However, Furo does not heed the warning, and he leaves the cooker on. Finally, the CONSUMED UNIT is equal to the RECHARED UNIT, and the SMART trips OFF the relay of the load.

#define USART_INTERRUPT
#define GSMBUFFER 100


volatile uint8_t gsmBufferSize;
volatile uint8_t terminalRxSize;
volatile char temp;
volatile char * gsmTokens[3];
volatile char gsmBuffer[GSMBUFFER];
volatile uint8_t gsmPingDuration=10;
volatile MESSAGE * gsmMessage;
volatile unsigned int gsmStopPingTime=0;

void messageMonitor(MESSAGE *,ELECTRICPOWER *);
void pingGSM(volatile MESSAGE * message, uint8_t period);





ISR(TIMER1_COMPB_vect){
time->elapsedTime < 0?(time->elapsedTime=0):(time->elapsedTime ++);
time->elapsedTime > 3600?(time->elapsedTime=0,gsmStopPingTime = gsmPingDuration)  :(time->elapsedTime ++);
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

if(!status->gsmPingWait ||  !status->gsmBusy)pingGSM(gsmMessage,gsmPingDuration);
}

ISR(ADC_vect){
	power->mainsCurrent = (uint8_t) ADCH;
	measurePower(power);
	toggle(DEVICE_PORT,POWER_LED);
}

#ifdef USART_INTERRUPT
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
			status->terminalIsReady=TRUE;
			status->displaySwitch=TRUE;
			status->gsmPingWait=FALSE;
			status->gsmBusy=FALSE;
			switchOff(DEVICE_PORT,BUSY_LED);
			return;
		}

		status->terminalIsReady = FALSE;
		status->gsmPingWait=TRUE;
		while(!(UCSR0A & _BV(UDRE0)));
		UDR0 = (char)temp;
		Message->data[terminalRxSize++] =toupper(temp);
		uartMetaData->rxSize = terminalRxSize;
		Message->displayMessageLength = terminalRxSize;
		Message->messageSource = TERMINAL;
		toggle(DEVICE_PORT,BUSY_LED);
}

ISR(USART3_RX_vect){
	gsmBuffer[gsmBufferSize++] = toupper(UDR3) ;
if(strstr(gsmBuffer,"OK") || strstr(gsmBuffer,"ERROR") || status->showAllGsmMessage == TRUE) 
	{
		status->gsmBusy = FALSE;
		if (strstr(gsmBuffer,"OK")) toggle(DEVICE_PORT,GSM_LED);
	}
else
		status->gsmBusy = TRUE;
}
#endif



void pingGSM(volatile MESSAGE * message, uint8_t period){

#define ATCOMMANDLENGTH 20 
if(time->elapsedTime > gsmStopPingTime ){
gsmStopPingTime=time->elapsedTime+period;
//#define DEVELOPMENT
#ifdef DEVELOPMENT
loadMessageBuffer(message,messageBank(AT),2);
#else
loadMessageBuffer(message,messageBank(ATCOMMANDLIST),ATCOMMANDLENGTH);
#endif
#undef DEVELOPMENT
status->loaded=TRUE;
}
}
void gsmResponseManager(MESSAGE * gsmToTerminalMessage,volatile char  gsmResponse[],volatile char *bufferForGSMTokens[]){

volatile char  gsmTemporaryBuffer[GSMBUFFER]="";
volatile char feedBackBuffer[GSMBUFFER]="";
uint8_t i=0;
for(;i<gsmBufferSize;i++)
gsmTemporaryBuffer[i]=gsmResponse[i];


if(strchr(gsmTemporaryBuffer,'#')){
extractGSMTokens(gsmTemporaryBuffer,bufferForGSMTokens,"#");
loadMessageBuffer(gsmToTerminalMessage,bufferForGSMTokens[1],strlen(bufferForGSMTokens[1]));
testGSM(gsmTemporaryBuffer,gsmBufferSize,bufferForGSMTokens);

strncat(feedBackBuffer,messageBank(OK),4);
strncat(feedBackBuffer,messageBank(COLUMN),1);
strncat(feedBackBuffer,gsmToTerminalMessage->data,gsmToTerminalMessage->displayMessageLength);

loadMessageBuffer(feedBack,feedBackBuffer,gsmBufferSize + 8);
sendGSMMessage(feedBack);
Message->messageSource=GSM;
	}
else
if(strstr(gsmTemporaryBuffer,messageBank(CMTI)) ||strstr(gsmTemporaryBuffer,messageBank(CMGR)) ){//Response to CMGL request
extractGSMTokens(gsmTemporaryBuffer,bufferForGSMTokens,",");
char * newMessageIndex;
newMessageIndex = strdup(messageBank(ATCOMMANDREAD));

strncat(newMessageIndex,bufferForGSMTokens[1],strlen(bufferForGSMTokens[1]));
strncat(newMessageIndex,messageBank(COMMAR),1);
strncat(newMessageIndex,"1",1);

sendGSMData(newMessageIndex,12);
newMessageIndex=NULL;
	}
#define DEVELOPMENT
#ifdef DEVELOPMENT
else
if(strstr(gsmTemporaryBuffer,messageBank(RING))){
displayMessage(gsmResponse,gsmBufferSize);
onAllDevices();
}
#endif
#undef DEVELOPMENT
else
displayMessage(gsmResponse,gsmBufferSize);

gsmBufferSize=0;
resetBuffer(gsmBuffer,GSMBUFFER);
resetBuffer(feedBackBuffer,GSMBUFFER);
}

int main(){
cli();
gsmBufferSize=0;
terminalRxSize=0;
gsmMessage=(MESSAGE *)malloc(sizeof(MESSAGE));
feedBack=(MESSAGE *)malloc(sizeof(MESSAGE));
initializeHardware();
initializeGSM();
initializeMeter();
status->displaySwitch=TRUE;
firstTime();
power->rechargedUnits=500;
status->verbose=TRUE;
status->showAllGsmMessage=FALSE;
sei();	
sendGSMData(messageBank(ATTEXTMODE),9);

while(1){
if(status->loaded && status->gsmPingWait==FALSE){
	sendGSMData(gsmMessage->data,gsmMessage->displayMessageLength);
	if(status->showAllGsmMessage)displayMessage(gsmMessage->data,gsmMessage->displayMessageLength);
	status->loaded=FALSE;
	if(status->terminalIsReady==FALSE) status->gsmIsReady=TRUE; 
}else status->gsmIsReady=FALSE; 
if(( (( gsmBufferSize > 0 ) && status->verbose) || strstr(gsmBuffer,messageBank(GHANACODE))||strstr(gsmBuffer,messageBank(RING)) ) && !status->gsmBusy){
status->displaySwitch=TRUE;
gsmResponseManager(Message,gsmBuffer,gsmTokens);
}

status->showAllGsmMessage == TRUE?( status->displaySwitch = TRUE):TRUE;

if(status->terminalIsReady==TRUE || status->gsmIsReady==TRUE)
{
	if(strstr(Message->data,messageBank(VERBOSE))){
		status->verbose = ~status->verbose;
		if(status->verbose == TRUE)buildStatus(messageBank(VERBOSE),7,TURNON);
		else
		if(status->verbose == FALSE)buildStatus(messageBank(VERBOSE),7,TURNOFF);
	}
	if(strstr(Message->data,messageBank(SHOWGSMALLMESSAGES))){
		status->showAllGsmMessage = ~status->showAllGsmMessage;
		if(status->showAllGsmMessage == TRUE)buildStatus(messageBank(SHOWGSMALLMESSAGES),4,TURNON);
		else
		if(status->showAllGsmMessage == FALSE)buildStatus(messageBank(SHOWGSMALLMESSAGES),4,TURNOFF);
	}

	messageMonitor(Message,power);
	meterMonitor(power);
	loadMonitor(Message);

	resetMessage(Message);
	resetMessage(gsmMessage);
	status->terminalIsReady=FALSE;
}
}
}
