/*
e*"The SMARTMETER measures the CONSUMED POWER". This done by the following steps:
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
	power->consumedUnits=0;
	power->rechargedUnits=0;
	power->remainingUnits=0;
	power->mainsCurrent=0;
	power->empty=FALSE;	
	power->emptyMessageIsDisplayed=FALSE;
	power->lowCreditMessageIsDisplayed=FALSE;
}

void measurePower( ELECTRICPOWER *pwr){
if( pwr->rechargedUnits > 0)
	{
	if(pwr->mainsCurrent >=0) pwr->consumedPower = pwr->mainsCurrent * 220;
	pwr->consumedUnits += pwr->consumedPower/10000; 
	pwr->remainingUnits=pwr->rechargedUnits  - pwr->consumedUnits;
	pwr->empty = FALSE;
	}
if(pwr->consumedUnits >= pwr->rechargedUnits && pwr->remainingUnits <=0 && pwr->emptyMessageIsDisplayed == FALSE)
	{
		pwr->empty=TRUE;
		pwr->rechargedUnits=0;
		pwr->remainingUnits=0;
		pwr->consumedPower=0;
		pwr->consumedUnits=0;
		pwr->emptyMessageIsDisplayed=TRUE;
		alert(messageBank(OUTOFCREDIT),9,3,3);
		loadMessageBuffer(feedBack,messageBank(OUTOFCREDIT),9);
		sendGSMMessage(feedBack);
		offAllDevices();
	}
else
if (pwr->remainingUnits <= 0.3 * pwr->consumedUnits && pwr->lowCreditMessageIsDisplayed == FALSE){
		pwr->lowCreditMessageIsDisplayed=TRUE;
		loadMessageBuffer(feedBack,messageBank(LOWCREDIT),10);
		sendGSMMessage(feedBack);
		alert(messageBank(LOWCREDIT),10,3,3);

}
}

void meterMonitor( ELECTRICPOWER * pwr)
{
char * temp;
UNITS tempUnits=0;
if(pwr->rechargeStringLength > 0)
	{
		tempUnits=0;
		temp = (char*)malloc(sizeof(char));
		strncpy(temp,pwr->rechargedUnitsString,pwr->rechargeStringLength);
		tempUnits= (UNITS)atoi(temp);
		pwr->rechargedUnits += tempUnits;
		pwr->remainingUnits += tempUnits;
		pwr->rechargeStringLength = 0;
		tempUnits=0;
		free(temp);
		pwr->lowCreditMessageIsDisplayed = FALSE;
		pwr->emptyMessageIsDisplayed = FALSE;
	}
}
