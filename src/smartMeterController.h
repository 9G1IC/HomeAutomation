#include <avr/io.h>
typedef  unsigned int UNITS;
typedef  unsigned int POWER;
typedef struct _power{
	volatile UNITS consumedUnits;
	volatile UNITS rechargedUnits;
	volatile int remainingUnits;
	volatile UNITS mainsCurrent;
	volatile UNITS consumedPower;
	volatile uint8_t unitPrice;
	volatile char rechargedUnitsString[3];
	volatile uint8_t rechargeStringLength;
	volatile uint8_t empty:1	;
	volatile uint8_t emptyMessageIsDisplayed:1;
	volatile uint8_t lowCreditMessageIsDisplayed:1;
} ELECTRICPOWER;


 ELECTRICPOWER * power;
void measurePower( ELECTRICPOWER*);
void initializeMeter();
void meterMonitor( ELECTRICPOWER *);
