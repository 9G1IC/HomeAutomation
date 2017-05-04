
//#define DEVELOPMENT
#ifdef DEVELOPMENT
void bufferUD0(){
		if(temp == '\r'){
		 	while(!(UCSR0A & _BV(UDRE0)));
			UDR0 = '\n';
		 	while(!(UCSR0A & _BV(UDRE0)));
			UDR0 = '\r';
		 	while(!(UCSR0A & _BV(UDRE0)));
			UDR0 = '>';
		        terminalRxSize=0;
			temp=' ';
			status->terminalIsReady=TRUE;
			status->displaySwitch=TRUE;
		 	status->gsmIsReady=TRUE;
		} else status->terminalIsReady = FALSE;
}
#endif
#undef DEVELOPMENT

#ifdef DEVELOPMENT
void test(uint8_t function){
	switch(function){
	case 0:
		//Ensure Message->displayMessageLength is counted correctly
	messageMonitor(Message,power);
	showGSMMessage(Message);
		break;
	case 1:
	getTime();
		break;	

	case 2:
	messageMonitor(Message,power);
	displayMessage(messageBank(PERIOD),10);
	write(messageBank(GSMDATA),8);
	write(messageBank( COLUMN ),1);
	write(Message->data,Message->displayMessageLength);
	write(messageBank( COLUMN ),1);
	displayNumber(Message->displayMessageLength);

	write(messageBank(COMMAND),7);
	write(messageBank(COLUMN),1);
	write(Message->command,Message->commandLength);
	write(messageBank( COLUMN ),1);
	displayNumber(Message->commandLength);

	write(messageBank(PARAMETER),9);
	write(messageBank( COLUMN ),1);
	write(Message->commandParameter,Message->commandParameterLength);
	write(messageBank( COLUMN ),1);
	displayNumber(Message->commandParameterLength);
	break;
	case 3:
		messageMonitor(Message,power);
		loadMonitor(Message);
		meterMonitor(power);
	break;
	case 4: 
	gsmResponseManager(Message->data,Message->displayMessageLength,gsmBuffer,gsmTokens);
	break;
	}
}
#endif
#undef DEVELOPMENT

