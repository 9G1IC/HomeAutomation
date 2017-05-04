#ifndef F_CPU 
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "messanger.h"
#include "smartMeterController.h"
#include "uart.h"
#include "gsm.h"
#include "hardware.h"
#include <avr/pgmspace.h> 
#include <ctype.h> 
//#define DEVELOPMENT
#undef DEVELOPMENT
#define USART_INTERRUPT
//Furo wants his smart meter to switch on his light bulb called, "bedroom"
/*
 * The message to be sent has to be in a format: "devicename devicecommandParameter"
 * This requires a function to merge the device name with the device commandParameter
 * This also requires a device map
 */

const char device1 [] PROGMEM ="BEDROOM";
const char device2 [] PROGMEM ="KITCHEN";
const char device3 [] PROGMEM ="DINING";
const char device4 [] PROGMEM ="PARLOR";
const char device5 [] PROGMEM ="BUZZER";
const char alldevices [] PROGMEM ="ALL";

const char message0 [] PROGMEM = "VERBOSE";
const char message1 [] PROGMEM ="TIME";
const char message2 [] PROGMEM ="STATUS";
const char message3 [] PROGMEM ="OKAY";
const char message4 [] PROGMEM ="RECHARGE";
const char message5 [] PROGMEM ="HELP";
const char message6 [] PROGMEM ="GSM";
const char message7 [] PROGMEM ="POWER";
const char devicehelp [] PROGMEM ="DLIST";
const char reset [] PROGMEM ="RESETGSM";
const char message8 [] PROGMEM = "INVALID CODE";
const char message9 [] PROGMEM ="SENDING";
const char message10 [] PROGMEM ="CONSUMED";
const char message11 [] PROGMEM = "ON";
const char message12 [] PROGMEM = "OFF";
const char message13 [] PROGMEM ="NO CREDIT";
const char message14 [] PROGMEM ="LOW CREDIT";
const char message15 [] PROGMEM ="DONE"; 

const char character0 [] PROGMEM = ">";
const char character1 [] PROGMEM =":";
const char character2 [] PROGMEM ="-";
const char character3 [] PROGMEM =" ";
const char character4 [] PROGMEM =".";
const char character5 [] PROGMEM =",";
const char character6 [] PROGMEM ="\"";
const char character7 [] PROGMEM ="#";
const char character8 [] PROGMEM =";";

const char welcome0 [] PROGMEM ="Welcome";
const char welcome1 [] PROGMEM ="Smart Meter";
const char welcome2 [] PROGMEM ="Type: Help";

const char atcommand0 [] PROGMEM ="at+cmgr=";
const char atcommand1 [] PROGMEM ="at+cmgl=\"rec unread\"";
const char atcommand2 [] PROGMEM ="at+cmgs=";
const char atcommand3 [] PROGMEM ="at+cmgf=1";
const char atcommand4 [] PROGMEM ="ate0";
const char atcommand5 [] PROGMEM ="at+cmgd=1,4";
const char atcommand6 [] PROGMEM ="atd";
const char atcommand7 [] PROGMEM ="at";

const char messageDestination [] PROGMEM ="+233204244772";
const char hiddenCommand1 [] PROGMEM ="DUMP";
const char ring [] PROGMEM ="RING";
const char cmti [] PROGMEM ="CMTI";
const char cmgr [] PROGMEM ="CMGR";
const char ghanacode [] PROGMEM ="+233";

const char detailedmessage0 [] PROGMEM = "SHOWS DETAILS OF GSM COMMUNICATION";
const char detailedmessage1 [] PROGMEM ="SHOWS ELAPSED TIME SINCE LAST BOOT";
const char detailedmessage2 [] PROGMEM ="SHOWS STATE OF DEVICES";
const char detailedmessage3 [] PROGMEM ="RESPONSE ON A VALID RECHARGE";
const char detailedmessage4 [] PROGMEM ="LOAD CREDIT";
const char detailedmessage5 [] PROGMEM ="SHOW THIS HELP MENU";
const char detailedmessage6 [] PROGMEM ="COMMUNICATE WITH GSM MODULE";
const char detailedmessage7 [] PROGMEM ="SHOWS POWER DETAILS";
const char detaileddevicehelp [] PROGMEM ="LISTS NAMES OF DEVICES";
const char detailedreset [] PROGMEM ="DELETES ALL INBOX SMS MESSAGES";
const char detailedalldevices [] PROGMEM ="CONTROLS ALL DEVICES AT ONCE";

#ifdef DEVELOPMENT
const char message16 [] PROGMEM ="GOT";
const char message17 [] PROGMEM ="FOUND";
const char message18 [] PROGMEM ="REMAINING";
const char message19 [] PROGMEM ="COMMAND";
const char message20 [] PROGMEM ="PARAMETER";
#endif
PGM_P const messageTable[]PROGMEM ={

 device1,
 device2,
 device3,
 device4,
 device5,
 alldevices,

 message0 ,
 message1 ,
 message2 ,
 message3 ,
 message4 ,
 message5 ,
 message6 ,
 message7,
 devicehelp,
 reset,
 message8,
 message9,
 message10,
 message11,
 message12,
 message13,
 message14,
 message15,

 character0,
 character1,
 character2,
 character3,
 character4,
 character5 ,
 character6 ,
 character7,
 character8,

 welcome0 ,
 welcome1 ,
 welcome2,

 atcommand0,
 atcommand1,
 atcommand2,
 atcommand3,
 atcommand4,
 atcommand5,
 atcommand6,
 atcommand7,

 messageDestination,
 hiddenCommand1,
 ring,
 cmti,
 cmgr,
 ghanacode,

 detailedmessage0 ,
 detailedmessage1 ,
 detailedmessage2 ,
 detailedmessage3 ,
 detailedmessage4 ,
 detailedmessage5 ,
 detailedmessage6 ,
 detailedmessage7,
 detaileddevicehelp,
 detailedreset,
 detailedalldevices,

#ifdef DEVELOPMENT
 message16,
 message17,
 message18,
 message19,
 message20
#endif
};


char BUF[20];
char * messageBank(uint8_t location){
strcpy_P(BUF,(char*)pgm_read_word(&messageTable[location]));
return (char *) &BUF;
}

void displayMessage(volatile char * message,uint8_t size){
	if(status->displaySwitch == 1){
	uart_tx(( char * )message,size);
	uart_tx("\n\r",2);
	}
}

void write(char * message,uint8_t size){
	if(status->displaySwitch==1)
	uart_tx(( char * )message,size);
}

void displayNumber(unsigned int number){
	char * temp;
	temp = malloc(sizeof(char));
		itoa(number,temp,10);
		displayMessage(temp,strlen(temp));
	free(temp);
}

void writeNumber(unsigned int number){
	char * temp;
	temp = malloc(sizeof(char));
		itoa(number,temp,10);
		write(temp,strlen(temp));
	free(temp);
}


void  getGSMMessage(char * data){
#ifndef USART_INTERRUPT
	uartMetaData = uart_rx();
	data = uartMetaData->data;
#endif
	Message->displayMessageLength = uartMetaData->rxSize;
}


void resetBuffer(volatile char *buffer, uint8_t size){
	uint8_t counter=0;
	for(;counter < size;counter++)buffer[counter]=' ';
}

void resetMessage(volatile MESSAGE *message){
	uint8_t counter=0;
	for(;counter < message->displayMessageLength ;counter++){
	if(counter < message->commandLength)message->command[counter]=' ';
	if(counter < message->commandParameterLength)message->commandParameter[counter]=' ';
	if(counter < message->displayMessageLength)message->data[counter]=' ';
	}
	message->commandParameterLength=0;
	message->commandLength=0;
	message->displayMessageLength=0;
}

void sendGSMMessage(MESSAGE * message){
#define DEFAULTMESSAGESIZE  77
	char  fullMessage [DEFAULTMESSAGESIZE]="";
	uint8_t fullMessageSize = 24 + message->displayMessageLength;
	status->displaySwitch=TRUE;
	sendGSMData(messageBank(ATTEXTMODE),9);

	strncat(fullMessage,messageBank(ATCOMMANDSEND),8);
	strncat(fullMessage,messageBank(QUOTE),1);
	strncat(fullMessage,messageBank(GSMMESSAGEDESTINATION),12);
	strncat(fullMessage,messageBank(QUOTE),1);
	strncat(fullMessage,"\n\r",2);
	sendGSMData(fullMessage,22);
	write(messageBank( SENDING ),7);
	write(messageBank(COLUMN),1);
	displayMessage( fullMessage,22);

	sendGSMData(message->data,message->displayMessageLength);
	UDR3=0x1A;
	displayMessage(message->data,message->displayMessageLength);

	resetBuffer(fullMessage,fullMessageSize);
	_delay_ms(5000);
}
#define DEVELOPMENT
#ifdef DEVELOPMENT
void showGSMMessage(MESSAGE * message){
write(message->data,message->displayMessageLength);
write(messageBank(COLUMN),1);
displayNumber(message->displayMessageLength);
}
#endif

void gsmMessageParser(MESSAGE *message){
//Check for device name
uint8_t commandSeparatorIndex = 0;
uint8_t parameterCounter=0;
uint8_t parameterSeparatorIndex=0;

if(message->displayMessageLength > 0){
for(commandSeparatorIndex=0;commandSeparatorIndex < message->displayMessageLength;commandSeparatorIndex++){
if(message->data[commandSeparatorIndex] == ' ') break;
message->command[commandSeparatorIndex] = message->data[commandSeparatorIndex];
}
message->commandLength = commandSeparatorIndex ;
parameterSeparatorIndex = commandSeparatorIndex + 1;
for(;parameterSeparatorIndex < message->displayMessageLength;parameterSeparatorIndex++){
message->commandParameter[parameterCounter++] = message->data[parameterSeparatorIndex];
}
message->commandParameterLength = parameterCounter;
}
}
void messageMonitor(MESSAGE *message,ELECTRICPOWER * measuredPower){
#define deviceIndex 6
#define commandSize 10
#define DEVICESIZE 4
#define MESSAGEOFFSET 44
#define ALLMESSAGEOFFSET MESSAGEOFFSET  + 11

uint8_t counter = 0;
uint8_t findCommand=6;
uint8_t commandIndex = findCommand + commandSize;

#ifndef USART_INTERRUPT
	getGSMMessage();
#endif
#undef DEVELOPMENT

gsmMessageParser(message);
//#define DEVELOPMENT
#ifdef DEVELOPMENT
showGSMMessage(message);
#endif
#undef DEVELOPMENT

while (findCommand <= commandIndex){
if(strncmp(message->command,messageBank(findCommand),message->commandLength)==0 && message->commandLength > 0)
{
switch(findCommand){
case SHOWPOWER:
	write(messageBank( CONSUMEDPOWER ),8);
	write(messageBank( COLUMN ),1);
	writeNumber(measuredPower->consumedUnits);
	write(messageBank( COLUMN ),1);
	writeNumber(measuredPower->remainingUnits);
	write(messageBank( DASH ),1);
	displayNumber(measuredPower->rechargedUnits);
	return;
case SHOWTIME:
	getTime();
	return;
case STATUS: 
	LOOP_DEVICES(){buildDeviceStatus(&Devices[counter]);} 
	return;
case OK:
	measuredPower->rechargeStringLength = 3;
	strncpy(measuredPower->rechargedUnitsString,"100",3);
	write(messageBank(RECHARGED),9);
	write(messageBank(COLUMN),1);
	write(measuredPower->rechargedUnitsString,measuredPower->rechargeStringLength);
	return;
case RECHARGED:
	sendGSMMessage(message);
	return;
case HELP:
	for(counter=deviceIndex;counter<commandIndex;counter++){
		write(messageBank(counter),strlen(messageBank(counter)));
		write(messageBank(SPACE),1);
		write(messageBank(ARROWHEAD),1);
		write(messageBank(SPACE),1);
		displayMessage(messageBank(counter + MESSAGEOFFSET),strlen(messageBank(counter + MESSAGEOFFSET)));
	}
	write(messageBank(ALL),strlen(messageBank(ALL)));
	write(messageBank(SPACE),1);
	write(messageBank(ARROWHEAD),1);
	write(messageBank(SPACE),1);
	displayMessage(messageBank(ALL + ALLMESSAGEOFFSET),strlen(messageBank(ALL +ALLMESSAGEOFFSET)));
	counter=0;
	return;
case GSMDATA:
	sendGSMData(message->commandParameter,message->commandParameterLength);
	return;
case RESETGSM:
	sendGSMData(messageBank(ATCOMMANDDELETE),11);
	sendGSMData(messageBank(ATTEXTMODE),9);
	return;
case DEVICEHELP:
	for(counter=0;counter<DEVICESIZE;counter++)
	displayMessage(messageBank(counter),strlen(messageBank(counter)));
	counter=0;
	return;
case INVALID:
	 alert(messageBank(INVALID),12,2,1);
	 return;
}
		}
findCommand++;
	}
}



void initializeGSM(){
initializeUart(defaultBaudRate);
Message =(MESSAGE *) malloc(sizeof(MESSAGE));

uartMetaData =(UART*) malloc(sizeof(UART));
uartMetaData->data =(char*) malloc(sizeof(char));
messageBankLength  =(uint8_t *) malloc(sizeof(uint8_t));
status = ( _STATUS*) malloc(sizeof( _STATUS));
status = &Status;

Message->displayMessageLength =0;
Message->commandLength = 0;
Message->commandParameterLength=0;

#ifdef DEVELOPMENT
*messageBankLength=MAX_MESSAGE_SIZE + 6	;
#else
*messageBankLength=MAX_MESSAGE_SIZE;
#endif
}


void alert(DISPLAYMESSAGE message,uint8_t size,uint8_t numberOfBeeps,uint8_t duration){
displayMessage(message,size);
beep(numberOfBeeps,duration,time);
}

void firstTime(){
	displayMessage((char*)messageBank(SPLASH1),strlen((const char*)messageBank(SPLASH1)));
	_delay_ms(1000);
	displayMessage((char*)messageBank(SPLASH2),strlen((const char*)messageBank(SPLASH2)));
	_delay_ms(1000);
	displayMessage((char*)messageBank(SPLASH3),strlen((const char*)messageBank(SPLASH3)));
}
void getTime(){
		writeNumber(time->hours);
		write(messageBank( COLUMN ),1);
		writeNumber(time->minutes);
		write(messageBank( COLUMN ),1);
		writeNumber(time->seconds);
		write(messageBank( DASH ),1);
		displayNumber(time->elapsedTime);
}


void loadMessageBuffer(volatile MESSAGE * message,char * source,uint8_t size){
				uint8_t i=0;
				for(;i<size;i++)
				message->data[i]=source[i];
				message->displayMessageLength=i;
}

void buildStatus(const char * text,uint8_t size,uint8_t state){
char * status;
uint8_t statusLength=size + 1;
status = (char *)malloc(sizeof(char));

strncat(status,text,size);
strncat(status,messageBank(COLUMN),1);

if(state == TURNON){
strncat(status,messageBank(TURNON),2);
statusLength +=3;
}else{
strncat(status,messageBank(TURNOFF),3); 
statusLength +=4;
}
strncat(status,messageBank(PERIOD),1);

loadMessageBuffer(feedBack,status,statusLength);

if(Message->messageSource == GSM)sendGSMMessage(feedBack);
else
if(Message->messageSource == TERMINAL) displayMessage(status,statusLength);

free(status);
}
