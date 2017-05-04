#ifndef F_CPU
#define F_CPU 16000000
#endif
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include "messanger.h"
#include "hardware.h"
#include "smartMeterController.h"
/*
Scenario 3: 
Now off, Furo  needs to continue cooking, but it is late and there is no way to get credit. He however has bought some recharge unit with code: "U123456". He sends the code "R U123456" as an sms message to the ELECTICTIY COMPANY'S GSM module.
The GSM Module of the electricity company receives the message and sends it to its MCU. The MCU compares the code with the codes on its database, and sends a confirmation signal of "Ok 20" to Furo's GSM module, and removes the code from its database. 
On receiving this signal Furo's GSM module sends the response to its MCU and it displays the recharged unit of "20 units" on the LCD, and it gives one short beep. The MCU then loads the RECHARGE UNIT of the meter with the received response. This makes the CONSUMED UNIT less than the RECHARED UNIT, and the MCU now re_engages the load relay and, Furo's cooking can resume. 
*/
/*
Scenario 4:
On this day, Furo needs to cook a fast meal for his visitor, and unfortunately, the CONSUMED UNITS go above the RECHARED UNITS, and the Smart meter trips off the load relay. Sadly, Furo does not have units. So, he guesses some code by sending "R U00001". The GSM Module of the electricity company receives the message and sends it to its MCU. The MCU compares the code with the codes on its database, and does not find a valid code. It then sends an error signal of "inv" to Furo's smart meters GSM module. On receiving this signal Furo's smart meter GSM module sends the response to its MCU and displays the error message of "invalid code" on the LCD, and give one long beep.

Scenario 6:
Furo needs to know the status of the connected loads. So, he sends "status" to the GSM module of the smart meter. On receiving this message, the MCU displays the "name " and "status " of the connected load on the LCD, and sends the same to Furo's GSM. Mr. Furo therefore receives:
"cooker OFF"
"bedroom bulb ON"
"Electric pump OFF"
*/			
void loadMonitor(MESSAGE * message){
uint8_t index = 0;
uint8_t status =-1;
//#define DEVELOPMENT
#ifdef DEVELOPMENT
showGSMMessage(message);
#endif
	while(message->commandLength > 0){
if(strstr(message->command, messageBank(DEVICE1))){
	if(strstr( message->commandParameter, messageBank(ON))){
       	changeDeviceState(Devices[0],DEVICE_ON); status=0; status=0;break;}
	if(strstr( message->commandParameter, messageBank(OFF))){
       	changeDeviceState(Devices[0],DEVICE_OFF); status=0;status=0;break;}
				}

if(strstr(message->command,messageBank(DEVICE2))){
		if(strstr( message->commandParameter, messageBank(ON))){
	       	changeDeviceState(Devices[1],DEVICE_ON); status=0;break;}
		if(strstr( message->commandParameter, messageBank(OFF))){
					       	changeDeviceState(Devices[1],DEVICE_OFF); status=0;break;}
				}

if(strstr( message->command,messageBank(DEVICE3))){
		if(strstr( message->commandParameter, messageBank(ON))){
	       	changeDeviceState(Devices[2],DEVICE_ON); status=0;break;}
		if(strstr( message->commandParameter, messageBank(OFF))){
	       	changeDeviceState(Devices[2],DEVICE_OFF); status=0;break;}
				}

if(strstr( message->command,messageBank(DEVICE4))){
		if(strstr( message->commandParameter, messageBank(ON))){
	       	changeDeviceState(Devices[3],DEVICE_ON); status=0;break;}
		if(strstr( message->commandParameter, messageBank(OFF))){
	       	changeDeviceState(Devices[3],DEVICE_OFF); status=0;break;}
				}

if(strstr( message->command,messageBank(ALL))){
		if(strstr( message->commandParameter, messageBank(ON))){ onAllDevices(); status=0;break;}
		if(strstr( message->commandParameter, messageBank(OFF))){offAllDevices(); status=0;break;}
				}

if (index++ > *messageBankLength )break;
	}
if(status==0) displayMessage(messageBank( DONE ),4);
*displaySwitch = 0;
}
