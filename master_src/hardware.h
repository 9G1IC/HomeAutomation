#include <avr/io.h>
#include "../../snippet/io.h"
#include <avr/eeprom.h>
// 4 Load outputs
#define PORTB_OUTPUT
#define LOAD1    OUTPUT_1
#define LOAD2    OUTPUT_2
#define LOAD3    OUTPUT_3
#define LOAD4    OUTPUT_4
#define DEVICE_PORT OUTPUT_PORT
#define POWER_LED OUTPUT_5
#define GSM_LED   OUTPUT_6 
#define BUSY_LED  OUTPUT_7
#define BUZZER	  OUTPUT_8
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
	DEVICESTATE  state;	
	uint8_t port;
	uint8_t  pin;
	uint8_t id;
}DEVICE;
uint8_t totalNumberOfDevices;
DEVICE * Devices[5] ;
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
DEVICESTATE getDeviceState(DEVICE);
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
