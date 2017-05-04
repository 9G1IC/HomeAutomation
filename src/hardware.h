#include <avr/io.h>
#include "../../snippet/io.h"

#define LOOP_DEVICES() for (counter=0;counter < 4;counter++)
//#include <avr/eeprom.h>
// 4 Load outputs
#define PORTB_OUTPUT
#define LOAD1    OUTPUT_1 //PB0
#define LOAD2    OUTPUT_2 //PB1
#define LOAD3    OUTPUT_3 //PB2
#define LOAD4    OUTPUT_4 //PB3

#define DEVICE_PORT OUTPUT_PORT
#define POWER_LED OUTPUT_5 //PB4
#define GSM_LED   OUTPUT_6 //PB5
#define BUSY_LED  OUTPUT_7 //PB6

#define BUZZER	  OUTPUT_8 //PB7
//Sensor
#define POWER_SENSOR_PORT PORTC
#define POWER_SENSOR 	  PINC4

#define PORTC_INPUT 
#define UP	BUTTON_1
#define DOWN	BUTTON_2
#define ENTER	BUTTON_3
#define CANCEL	BUTTON_4
#define POWER_SWITCH	BUTTON_5

//Device
typedef enum _devicecommandParameter {DEVICE_ON,DEVICE_OFF} DEVICESTATE;
typedef const char  DEVICENAME;//Should be an eeprom variable
typedef struct device {
	DEVICESTATE  state:1;	
	uint8_t  pin:3;
	uint8_t id:3;
}DEVICE;

uint8_t totalNumberOfDevices;
DEVICE  devices[5] ;
DEVICE  * Devices ;
//Sampler Resolution: This is need to set the sampler for  the ADC of the power meter. 
unsigned int adcSamplerInterval;

void initADC();
void initTimer();
void initButtons();
void initLeds();
void initializeHardware();
void initializeDevice();
void setDeviceName(DEVICE*,char []);
void changeDeviceState(DEVICE *,DEVICESTATE);
void offAllDevices();
void onAllDevices();


//Timer
typedef struct _time{
 uint8_t seconds ;
 uint8_t minutes ;
 uint8_t hours ;
 unsigned int  elapsedTime;
}TIME;

void beep(uint8_t numberOfBeeps,uint8_t duration,TIME * time);

TIME * time;
DEVICE * Buzzer;
void buildDeviceStatus(DEVICE * device);
