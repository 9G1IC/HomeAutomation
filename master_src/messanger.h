#include<avr/io.h>

typedef char * DISPLAYMESSAGE;
typedef char * EEPROMMESSAGE;
typedef char * GSMMESSAGE ;
//TODO: make display message an EEROM variable
typedef struct _messageFormat {
	char  command[20];
	char commandParameter[20];
	char data[40];
	uint8_t commandLength;
	uint8_t commandParameterLength;
	uint8_t displayMessageLength;
} MESSAGE;

void getGSMMessage();

void gsmMessageParser(MESSAGE *);
void sendGSMMessage(MESSAGE *);
void initializeGSM();
void showGSMMessage(MESSAGE *);
void firstTime();
void displayMessage(volatile char * , uint8_t);
void write(char *, uint8_t);
void displayNumber(unsigned int );
void writeNumber(volatile unsigned int );
void getTime();
MESSAGE * Message;

enum EEPROMMESSAGEINDEX {ON=0,OFF,STATUS,OK,INVALID,ARROWHEAD,OUTOFCREDIT,RECHARGED,HELP,GOT,FOUND,SENDING,CONSUMEDPOWER,REMAININGPOWER,COLUMN,DASH,SHOWPOWER,DONE,SPACE,GSMDATA,COMMAND,PERIOD,PARAMETER,SHOWTIME,COMMAR,QUOTE,SPLASH1,SPLASH2,SPLASH3,DEVICE1,DEVICE2,DEVICE3,DEVICE4,DEVICE5,DEVICEHELP,ALL,ATCOMMANDREAD,ATCOMMANDLIST,ATCOMMANDSEND,ATTEXTMODE,ATECHOOFF,GSMMESSAGEDESTINATION};


char * messageBank(uint8_t);
typedef enum _bool{FALSE=0,TRUE}bool;

void alert(DISPLAYMESSAGE ,uint8_t,uint8_t ,uint8_t );
uint8_t * messageBankLength;
void loadMonitor(MESSAGE *);
uint8_t * displaySwitch;
