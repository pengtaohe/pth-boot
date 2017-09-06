#include "shell.h"

#define ROOT       "\rpth-boot"

extern CMD_TABLE cmdSetBaud;
extern CMD_TABLE cmdReset;
extern CMD_TABLE cmdDownApp;
extern CMD_TABLE cmdStartApp;


/* pth-boot ½Úµã */
DefShellCmd( cmdVersion, "ver", funcVersion, "show version.", "", NULL, &cmdSetBaud ) 
{
	printf("\r\npth-boot Version: V1.0\r\n");
}	

DefShellCmd( cmdSetBaud, "setbaud", funcSetBaud, "set baud rate", "setbaud rate\n\rParameter: 9600|19200|38400|57600|115200", &cmdVersion, &cmdReset )
{
#if 0
	UINT32 baud;
	if((vty->argc == 0)||((strcmp(vty->argv[0],"?") == 0 )||(strcmp(vty->argv[0],"help") == 0 )))
	{
		printf( "setbaud rate\n\rParameter: 9600|19200|38400|57600|115200 \n\r" );
		return;
	}
	
	if( vty->argc != 1 ) vty_printf( "parameter error\n\r" );
	else
	{
		sscanf( vty->argv[0],"%ld",&baud );
		if(( 9600 == baud)||( 19200 == baud)||( 38400 == baud)||( 57600 == baud)||( 115200 == baud))
		{
			CONSOLE_BAUDRATE = baud;
			UART_Configure(CONSOLE_BAUDRATE, BOARD_MCK);
			printf("CONSOLE_BAUDRATE = %ld\r\n",CONSOLE_BAUDRATE);
		}
		else
		{
			printf( "setbaud rate\n\rParameter: 9600|19200|38400|57600|115200 \n\r" );
		}
	}	
#endif
}

DefShellCmd( cmdReset, "reset", funcReset, "reset.", "", &cmdSetBaud, &cmdDownApp )
{
#if 0
	if((strcmp(vty->argv[0],"?") == 0 )||(strcmp(vty->argv[0],"help") == 0 ))
	{
	    printf("reset local.\n\r");
		return;
	}
	else if( (vty->argc == 0 )||(strcmp(vty->argv[0],"local")==0)  )
	{
		printf("\n\rReset System...\n");
		reset_device();
	}   
	else
	{
		printf( "Parameter error!reset local.\n\r" );
	}
#endif
}

extern void load_app(char type);
DefShellCmd( cmdDownApp, "downapp", funcDownApp, "download app.", "", &cmdReset, &cmdStartApp )
{
	if(1 != vty->argc)
	{
		printf("	argument error\r\n");
	}
	else
	{
		if((strcmp(vty->argv[0],"?") == 0 )||(strcmp(vty->argv[0],"help") == 0 ))
		{
		    printf(" argv:\r\n");
			printf("    sram    download app of sram-startup\r\n");
			printf("    flash   download app of flash-startup\r\n");
			return;
		}
		else if(strcmp(vty->argv[0],"sram") == 0 )
		{
			load_app(1);
		}
		else if(strcmp(vty->argv[0],"flash") == 0 )
		{
			load_app(2);
		}
		else
		{
			printf("	argument error\r\n");
		}
	}

	return;
}

extern void start_app(char type);
DefShellCmd( cmdStartApp, "startapp", funcStartApp, "start app.", "", &cmdDownApp, NULL )
{
	if(1 != vty->argc)
	{
		printf("	argument error\r\n");
	}
	else
	{
		if((strcmp(vty->argv[0],"?") == 0 )||(strcmp(vty->argv[0],"help") == 0 ))
		{
		    printf(" argv:\r\n");
			printf("    sram    startup app from sram\r\n");
			printf("    flash   startup app from flash\r\n");
			return;
		}
		else if(strcmp(vty->argv[0],"sram") == 0 )
		{
			start_app(1);
		}
		else if(strcmp(vty->argv[0],"flash") == 0 )
		{
			start_app(2);
		}
		else
		{
			printf("	argument error\r\n");
		}
	}

	return;
}

extern NODE_TABLE node_Root;

NODE_TABLE node_Root = { NULL,NULL,NULL,NULL,&cmdVersion, ROOT };

