#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>

#include "hardware.h"
#include "../../snippet/io.h"
#include "messanger.h"

//Device1,Device2,Device3,Device4,Buzzer,up,down,enter,cancel,power_led,busy_led,gsm_led,current_sensor.
//Measure Power
//Sample ADC from POWER_SENSOR at a given interval. 
void initADC(){
	//This sets the ADC to sample at the rate set by Timer0A. It measures the current from the connected current transformer. This value multiplied by the voltage give the consumed power 
	//Enable ADC
	ADCSRA |= _BV(ADEN);
	//Enable ADC Automatic Trigger
	//ADATE
	ADCSRA |= _BV(ADATE);
	//Use the lowest time-> /128
	ADCSRA |= _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);//125k
	//Compare match for time-> 0 A
	ADCSRB |= _BV(ADTS2) | _BV(ADTS0);	
	//Read the values of ADCH and ADHL in the interrupts
	ADCSRA |= _BV(ADIE);
	//TODO
	//Have an ISR to handle the events
	//Select ADC4 and AREF
	ADMUX |= _BV(MUX2);
}

void initTimer(){
//Sample at set interval
OCR1A = (F_CPU/(2 * 256 * 2)) - 1;
TCCR1B |= _BV(WGM12) | _BV(CS12) | _BV(CS10);
TIFR1 |= _BV(OCF1A);
TIMSK1 |= _BV(OCIE1B);//Enable Interrupts on 0 A on compare match
time = malloc(sizeof(TIME));
}

void initButtons(){
	BUTTON_PORT |= _BV(UP) | _BV(DOWN) | _BV(ENTER) | _BV(CANCEL) | _BV(POWER_SWITCH);
}
void initLeds(){
	OUTPUT_DDR |= _BV(POWER_LED) | _BV(GSM_LED) | _BV(BUSY_LED);
	OUTPUT_PORT |= _BV(POWER_LED) | _BV(GSM_LED) | _BV(BUSY_LED);
}

void initializeDevice(){
totalNumberOfDevices = 4;

Devices[0]=(DEVICE*)malloc(sizeof(DEVICE));
Devices[0]->port=DEVICE_PORT;
Devices[0]->state=DEVICE_OFF;
Devices[0]->pin=LOAD1;
Devices[0]->id=30;

Devices[1]=(DEVICE*)malloc(sizeof(DEVICE));
Devices[1]->port=DEVICE_PORT;
Devices[1]->state=DEVICE_OFF;
Devices[1]->pin=LOAD2;
Devices[1]->id=31;

Devices[2]=(DEVICE*)malloc(sizeof(DEVICE));
Devices[2]->port=DEVICE_PORT;
Devices[2]->state=DEVICE_OFF;
Devices[2]->pin=LOAD3;
Devices[2]->id=32;

Devices[3]=(DEVICE*)malloc(sizeof(DEVICE));
Devices[3]->port=DEVICE_PORT;
Devices[3]->state=DEVICE_OFF;
Devices[3]->pin=LOAD4;
Devices[3]->id=33;

Devices[4]=(DEVICE*)malloc(sizeof(DEVICE));
Devices[4]->port=DEVICE_PORT;
Devices[4]->state=DEVICE_OFF;
Devices[4]->pin=BUZZER;
Devices[4]->id=34;

Buzzer = Devices[4];
}
void initializeHardware(){
	initADC();
	initButtons();
	initLeds();
	initTimer();
	initializeDevice();
}
//Display the information on the screen
//Eg: Bedroom:ON
/*
Scenario 5: 
On this day, Furo is on a plane to Nigeria, when he remembers that he has left his electric cooker ON. He quickly sends an sms message containing; "cooker OFF", to the smart meter's GSM module. On receiving this message, the MCU turns OFF the signal to the cooker relay, at the same time-> it sends a confirmation to Furo containing the message "cooker:OFF". Now, Furo heaves a sigh of relief. 
 * */
void buildDeviceStatus(DEVICE * device){
	if(device->id >=30 && device->id <= 34)
	write(messageBank(device->id),strlen(messageBank(device->id)));
	write(messageBank(COLUMN),1);
	if(device->state == DEVICE_ON)
	write(messageBank( ON ),3);
	else
	write(messageBank( OFF ),4);
	displayMessage(messageBank(PERIOD),1);
}

void changeDeviceState(DEVICE * device,DEVICESTATE  state){
	switch(state){
		case DEVICE_ON:
	device->port |= _BV(device->pin);
			break;
		case DEVICE_OFF:
	device->port |= (0 << device->pin);
			break;
	}
device->state = state;
buildDeviceStatus(device);
}

void onAllDevices(){
	uint8_t counter = 0;
for  (;counter < totalNumberOfDevices;counter++)
	changeDeviceState(Devices[counter],DEVICE_ON);
}
void offAllDevices(){
	uint8_t counter = 0;
for  (;counter < totalNumberOfDevices;counter++)
	changeDeviceState(Devices[counter],DEVICE_OFF);

}


static int endTime= 0;	
static uint8_t beepCounter =0;
static uint8_t stopInterval = 2;
uint8_t beepSwitch=0;

void beep(uint8_t numberOfBeeps,uint8_t durationOfBeep,TIME * time){

*displaySwitch=0;
if(time->elapsedTime > endTime){
	endTime = time->elapsedTime + stopInterval;
	if(beepSwitch ^= beepSwitch){
		changeDeviceState(Buzzer,DEVICE_ON);
		beepCounter++;
	}
	else changeDeviceState(Buzzer,DEVICE_OFF);
	if(beepCounter >=numberOfBeeps)beepCounter=0;
}

}
