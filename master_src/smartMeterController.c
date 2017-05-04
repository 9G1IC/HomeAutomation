/*
 *"The SMARTMETER measures the CONSUMED POWER". This done by the following steps:
 * 1. The ADC Measures the consumed current at an interval of 1 second
 * 2. The ISR computes the consumed power by mainsCurrent * mainsVoltage

 Scenario 2
Furo cooks with an electric cooker which CONSUMES tonnes of POWER. The SMART METER MEASURES the CONSUMED POWER, and converts it to CONSUMED UNITS. The CONSUMED UNITS are deducted from the RECHARED UNITS to give the REMAINING UNITS. This process goes on until the CONSUMED UNITS reaches the CRITICAL UNITS, at which point, the meter alerts Furo, by sending 3 LONG BEEPS and displaying "LOW CREDIT" on the display module; a PC monitor. However, Furo does not heed the warning, and he leaves the cooker on. Finally, the CONSUMED UNIT is equal to the RECHARED UNIT, and the SMART trips OFF the relay of the load.

Scenario 3: 
Now off, Furo  needs to continue cooking, but it is late and there is no way to get credit. He however has bought some recharge unit with code: "U123456". He sends the code "R U123456" as an sms message to the ELECTICTIY COMPANY'S GSM module.
The GSM Module of the electricity company receives the message and sends it to its MCU. The MCU compares the code with the codes on its database, and sends a confirmation signal of "Ok 20" to Furo's GSM module, and removes the code from its database. 
On receiving this signal Furo's GSM module sends the response to its MCU and it displays the recharged unit of "20 units" on the LCD, and it gives one short beep. The MCU then loads the RECHARGE UNIT of the meter with the received response. This makes the CONSUMED UNIT less than the RECHARED UNIT, and the MCU now re-engages the load relay and, Furo's cooking can resume. 
*/


#include "smartMeterController.h"
#include "hardware.h"
#include "messanger.h"
#include <avr/io.h>
#include <stdlib.h>
void initializeMeter(){
	power = (ELECTRICPOWER*)malloc(sizeof(ELECTRICPOWER));
	power->rechargedUnitsString = (char *) malloc(sizeof(char));
	power->consumedUnits^=power->consumedUnits;
	power->rechargedUnits ^=power->rechargedUnits ;
	power->remainingUnits^=power->remainingUnits;
	power->reserveUnit^=power->reserveUnit;
	power->mainsCurrent ^=power->mainsCurrent ;
	power->consumedPower ^=power->consumedPower ;
	power->mainsVoltage = 220;
}

void measurePower(ELECTRICPOWER *pwr){
if( pwr->rechargedUnits > 0){
if(pwr->mainsCurrent >=0) pwr->consumedPower += (pwr->mainsCurrent/100) *  pwr->mainsVoltage;
pwr->consumedUnits < 0?pwr->consumedUnits = 0:(pwr->consumedUnits += (pwr->consumedPower) * (time->elapsedTime)/3600);
pwr->remainingUnits=pwr->rechargedUnits  - pwr->consumedUnits;
pwr->empty = 0;
}
if(pwr->remainingUnits <= 0 || pwr->consumedUnits >= pwr->rechargedUnits)
{
	pwr->empty==1?(1):(pwr->rechargeStringLength=0);
	pwr->empty=1;
	power->consumedUnits^=power->consumedUnits;
	power->rechargedUnits ^=power->rechargedUnits ;
	power->remainingUnits^=power->remainingUnits;
	power->consumedPower ^=power->consumedPower ;
}
}

void meterMonitor(ELECTRICPOWER * pwr){
char * temp;
if(pwr->rechargeStringLength > 0){
temp = (char * )malloc(sizeof(char));
strncpy(temp,pwr->rechargedUnitsString,pwr->rechargeStringLength);
pwr->rechargedUnits = (unsigned int)atoi(temp);
free(temp);
pwr->rechargeStringLength = 0;
}
if(pwr->empty==1){
	offAllDevices();
	alert(messageBank(OUTOFCREDIT),9,3,3);
	*displaySwitch = 0;
	pwr->empty=0;
}
	}
