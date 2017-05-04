#include<avr/io.h>

#undef DEVELOPMENT
#define TRUE 1
#define FALSE 0
#define GSM 2
#define TERMINAL 3
#define MAX_MESSAGE_SIZE 50

typedef char * DISPLAYMESSAGE;
typedef char * EEPROMMESSAGE;
typedef char * GSMMESSAGE ;

typedef struct _messageFormat {
	char  command[20];
	char commandParameter[20];
	char data[50];
	int8_t commandLength:5;
	uint8_t commandParameterLength:5;
	uint8_t displayMessageLength:5;
	uint8_t messageSource:2;
} MESSAGE;

void getGSMMessage();

void resetMessage(volatile MESSAGE *message);
void resetBuffer(volatile char *buffer,uint8_t size);
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
void loadMessageBuffer(volatile MESSAGE * message,char * source,uint8_t size);
void loadMonitor(MESSAGE *);
void buildStatus(const char * text,uint8_t size,uint8_t state);
MESSAGE * Message;
MESSAGE * feedBack;

enum FLASHMEMORYINDEX {
	DEVICE1=0,
	DEVICE2,
	DEVICE3,
	DEVICE4,
	DEVICE5,
	ALL,

	VERBOSE,
	SHOWTIME,
	STATUS,
	OK,
	RECHARGED,
	HELP,
	GSMDATA,
	SHOWPOWER,
	DEVICEHELP,
	RESETGSM,

	INVALID,
	SENDING,
	CONSUMEDPOWER,
	TURNON,
	TURNOFF,
	OUTOFCREDIT,
	LOWCREDIT,
	DONE,

	ARROWHEAD,
	COLUMN,
	DASH,
	SPACE,
	PERIOD,
	COMMAR,
	QUOTE,
	HASH,
	SEMICOLUMN,

	SPLASH1,
	SPLASH2,
	SPLASH3,

	ATCOMMANDREAD,
	ATCOMMANDLIST,
	ATCOMMANDSEND,
	ATTEXTMODE,
	ATECHOOFF,
	ATCOMMANDDELETE,
	ATCALL,
	AT,

	GSMMESSAGEDESTINATION,
	SHOWGSMALLMESSAGES,
	RING,
	CMTI,
	CMGR,
	GHANACODE,
#ifdef DEVELOPMENT
	GOT,
	FOUND,
	REMAININGPOWER,
	COMMAND,
	PARAMETER,
#endif
};
char * messageBank(uint8_t);

void alert(DISPLAYMESSAGE ,uint8_t,uint8_t ,uint8_t );
uint8_t * messageBankLength;

typedef struct {
volatile uint8_t terminalIsReady:1;
volatile uint8_t gsmIsReady:1;
volatile uint8_t gsmPingWait:1;
volatile uint8_t gsmBusy:1;
volatile uint8_t verbose:1;
volatile uint8_t displaySwitch:1;
volatile uint8_t loaded:1;
volatile uint8_t showAllGsmMessage:1;
}  _STATUS;
_STATUS Status,*status;

