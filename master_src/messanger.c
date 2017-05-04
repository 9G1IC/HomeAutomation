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
const char devicehelp [] PROGMEM ="DLIST";
const char alldevices [] PROGMEM ="ALL";

const char message0 [] PROGMEM ="ON";
const char message1 [] PROGMEM ="OFF";
const char message2 [] PROGMEM ="STATUS";
const char message3 [] PROGMEM ="OKAY";
const char message4 [] PROGMEM ="INVALID";
const char message5 [] PROGMEM =">";
const char message6 [] PROGMEM ="NO CREDIT";
const char message7 [] PROGMEM ="RECHARGE";
const char message8 [] PROGMEM ="HELP";
const char message9 [] PROGMEM ="GOT";
const char message10 [] PROGMEM ="FOUND";
const char message11 [] PROGMEM ="SENDING";
const char message12 [] PROGMEM ="CONSUMED";
const char message13 [] PROGMEM ="REMAINING";
const char message14 [] PROGMEM =":";
const char message15 [] PROGMEM ="-";
const char message16 [] PROGMEM ="POWER";
const char message17 [] PROGMEM ="DONE";
const char message18 [] PROGMEM =" ";
const char message19 [] PROGMEM ="GSM DATA";
const char message20 [] PROGMEM ="COMMAND";
const char message21 [] PROGMEM =".";
const char message22 [] PROGMEM ="PARAMETER";
const char message23 [] PROGMEM ="TIME";
const char message24 [] PROGMEM =",";
const char message25 [] PROGMEM ="\"";


const char welcome0 [] PROGMEM ="Project Group 1";
const char welcome1 [] PROGMEM ="Welcome";
const char welcome2 [] PROGMEM ="Type: Help";

const char atcommand0 [] PROGMEM ="at+cmgr=";
const char atcommand1 [] PROGMEM ="at+cmgl=\"rec unread\"";
const char atcommand2 [] PROGMEM ="at+cmgs=";

const char atcommand3 [] PROGMEM ="at+cmgf=1";
const char atcommand4 [] PROGMEM ="ate0";
const char messageDestination [] PROGMEM ="\"0542651740\"";

PGM_P const messageTable[]PROGMEM ={
 message0 ,
 message1 ,
 message2 ,
 message3 ,
 message4 ,
 message5 ,
 message6 ,
 message7 ,
 message8 ,
 message9 ,
 message10 ,
 message11 ,
 message12 ,
 message13 ,
 message14 ,
 message15 ,
 message16 ,
 message17 ,
 message18 ,
 message19 ,
 message20 ,
 message21 ,
 message22 ,
 message23 ,
 message24 ,
 message25 ,
 welcome0 ,
 welcome1 ,
 welcome2,
 device1,
 device2,
 device3,
 device4,
 device5,
 devicehelp,
 alldevices,
 atcommand0,
 atcommand1,
 atcommand2,
 atcommand3,
 atcommand4,
 messageDestination
};

char BUF[20];
char * messageBank(uint8_t location){
strcpy_P(BUF,(char*)pgm_read_word(&messageTable[location]));
return (char *) &BUF;
}

void displayMessage(volatile char * message,uint8_t size){
	if(*displaySwitch == 1){
	uart_tx(( char * )message,size);
	uart_tx("\n\r",2);
	}
}

void write(char * message,uint8_t size){
	if(*displaySwitch==1)
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

void sendGSMMessage(MESSAGE * message){
	char * fullMessage;
	uint8_t fullMessageSize = 23 + message->displayMessageLength;
	fullMessage = (char * )malloc(sizeof(char));

	strncat(fullMessage,messageBank(ATCOMMANDSEND),8);
	strncat(fullMessage,messageBank(GSMMESSAGEDESTINATION),12);
	strncat(fullMessage,messageBank(COMMAR),1);
	strncat(fullMessage,messageBank(QUOTE),1);
	strncat(fullMessage,message->data,message->displayMessageLength);
	strncat(fullMessage,messageBank(QUOTE),1);

	write(messageBank( SENDING ),7);
	write(messageBank(COLUMN),1);
	displayMessage( fullMessage,fullMessageSize);

	sendGSMData(fullMessage,fullMessageSize);
	message->displayMessageLength=fullMessageSize;

	free(fullMessage);
}
#define DEVELOPMENT
#ifdef DEVELOPMENT
void showGSMMessage(MESSAGE * message){
write(messageBank( GOT ),3);
write(messageBank(COLUMN),1);
write(message->data,message->displayMessageLength);
write(messageBank(COLUMN),1);
displayNumber(message->displayMessageLength);
}
#endif

void gsmMessageParser(MESSAGE *message){
//Check for device name
uint8_t commandSeparatorIndex = 0;
uint8_t commandParameterLength =0;
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
if(message->data[parameterSeparatorIndex] == ' ') break;
message->commandParameter[parameterCounter++] = message->data[parameterSeparatorIndex];
}
message->commandParameterLength = parameterCounter;
}

}

#undef DEVELOPMENT
#ifdef DEVELOPMENT
void resetMessage(MESSAGE *message){
	uint8_t counter=0;
	for(;counter < message->commandLength && counter < message->commandParameterLength && counter < message->data;counter++){
	if(counter < message->commandLength)message->command[counter]=0;
	if(counter < message->commandParameterLength)message->commandParameter[counter]=0;
	if(counter < message->displayMessageLength)message->data[counter]=0;
	}
	message->commandParameterLength=0;
	message->commandLength=0;
	message->displayMessageLength=0;
}
#endif
void messageMonitor(MESSAGE *message,ELECTRICPOWER * measuredPower){
uint8_t status =-1;
uint8_t index = 0;
uint8_t allDevicesIncludingBuzzer = totalNumberOfDevices + 1;
#ifndef USART_INTERRUPT
	getGSMMessage();
#endif

gsmMessageParser(message);
//#define DEVELOPMENT
#ifdef DEVELOPMENT
showGSMMessage(message);
#endif
while(message->commandLength > 0){
if(strstr(message->command,messageBank(RECHARGED))){ sendGSMMessage(message); status=0; break;}

	if(strstr( message->command,  messageBank(STATUS))){
				buildDeviceStatus(Devices[0]);
				buildDeviceStatus(Devices[1]);
				buildDeviceStatus(Devices[2]);
				buildDeviceStatus(Devices[4]);
						status=0;
						break;}

	if( strstr( message->command,  messageBank(INVALID))){
		alert(messageBank(INVALID),message->commandLength,2,1);
			offAllDevices();
			status=0;
		break;}

if( strstr( message->command,  messageBank( OK ))){
measuredPower->rechargedUnitsString = (char*)message->commandParameter;
measuredPower->rechargeStringLength = Message->commandParameterLength;
write(messageBank(RECHARGED),9);
write(messageBank(COLUMN),1);
write(measuredPower->rechargedUnitsString,measuredPower->rechargeStringLength);
		alert(messageBank(OK),1,1,1);
		status=0;
		break;}

	if( strstr( message->command,  messageBank(SHOWPOWER))){
				write(messageBank( CONSUMEDPOWER ),8);
				write(messageBank( COLUMN ),1);
				writeNumber(measuredPower->consumedPower);
				write(messageBank( SPACE ),1);
				writeNumber(measuredPower->consumedUnits);
				write(messageBank( COLUMN ),1);
				writeNumber(measuredPower->remainingUnits);
				write(messageBank( DASH ),1);
				displayNumber(measuredPower->rechargedUnits);
				status=0;
			break;}

	if( strstr( message->command,  messageBank( SHOWTIME) )){
	       	getTime(); break;}

	if( strstr( message->command,  messageBank( DEVICEHELP) )){
				displayMessage(messageBank(DEVICE1),7);
				displayMessage(messageBank(DEVICE2),7);
				displayMessage(messageBank(DEVICE3),7);
				displayMessage(messageBank(DEVICE4),7);
				status=0;
			break;}

	if(strstr(message->command, messageBank( HELP) )){
	write(messageBank(SHOWPOWER),strlen(messageBank(SHOWPOWER)));
	displayMessage(messageBank(PERIOD),1);

	write(messageBank(SHOWTIME),strlen(messageBank(SHOWTIME)));
	displayMessage(messageBank(PERIOD),1);

	write(messageBank(RECHARGED),strlen(messageBank(RECHARGED)));
	displayMessage(messageBank(PERIOD),1);

	write(messageBank(DEVICEHELP),strlen(messageBank(DEVICEHELP)));
	displayMessage(messageBank(PERIOD),1);

	write(messageBank(OK),strlen(messageBank(OK)));
	displayMessage(messageBank(PERIOD),1);

	write(messageBank(INVALID),strlen(messageBank(INVALID)));
	displayMessage(messageBank(PERIOD),1);

	write(messageBank(HELP),strlen(messageBank(HELP)));
	displayMessage(messageBank(PERIOD),1);

	write(messageBank(ALL),strlen(messageBank(ALL)));
	displayMessage(messageBank(PERIOD),1);

	status=0;
					break;
				}
	if (index++ >= *messageBankLength )break;
	}

if(status==0) {
displayMessage(messageBank( DONE ),4);
status=1;
}
}


void initializeGSM(){
initializeUart(defaultBaudRate);
Message =(MESSAGE *) malloc(sizeof(MESSAGE));

uartMetaData =(UART*) malloc(sizeof(UART));
uartMetaData->data =(char*) malloc(sizeof(char));
messageBankLength  =(uint8_t *) malloc(sizeof(uint8_t));
displaySwitch = (uint8_t *) malloc(sizeof(uint8_t));

Message->displayMessageLength =0;
Message->commandLength = 0;
Message->commandParameterLength=0;

*messageBankLength=42;
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
		//Ensure time->is correct
		writeNumber(time->hours);
		write(messageBank( COLUMN ),1);
		writeNumber(time->minutes);
		write(messageBank( COLUMN ),1);
		writeNumber(time->seconds);
		write(messageBank( DASH ),1);
		displayNumber(time->elapsedTime);
}
