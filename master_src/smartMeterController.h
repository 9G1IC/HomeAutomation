#include <avr/io.h>
typedef  int UNITS;
typedef  int POWER;
typedef struct _power{
	UNITS consumedUnits;
	UNITS rechargedUnits;
	UNITS remainingUnits;
	UNITS reserveUnit;
	UNITS mainsCurrent;
	UNITS consumedPower;
	uint8_t  mainsVoltage;
	uint8_t unitPrice;
	char * rechargedUnitsString;
	uint8_t rechargeStringLength;
	uint8_t empty;
} ELECTRICPOWER;


ELECTRICPOWER * power;
uint8_t powerToUnitConverter(ELECTRICPOWER);
void criticalUnitMonitor();
void measurePower(ELECTRICPOWER*);
uint8_t criticalUnitThresholdPercentage;
void initializeMeter();
void getConsumedPower(ELECTRICPOWER *);
void meterMonitor(ELECTRICPOWER *);
