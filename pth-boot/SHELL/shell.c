#include "shell.h"

#define CFG_CBSIZE  255

#define print_string	printf

extern NODE_TABLE node_Root;

void Exec_cmd( VTY_STRUCT *vty, char *cmd );

void PrintLoginMsg( void )
{
	char buf_print[128] = "\r\nWelcome to use the \"pth-boot\"\r\n";
	print_string( buf_print );
}

void StrToLower( char *str )
{
	int i;

	i = 0;
	while( str[i] )
	{
		if( str[i] >= 'A' && str[i] <= 'Z' ) 
			str[i] = str[i] - 'A' + 'a';
		i++;
	}
}

void ShellLogIn( VTY_STRUCT *vty, char *cmd )
{

	VTY_ACCOUNT *pAccount;
	
	if( cmd == NULL ) return ;

	print_string( "\n\r");

	switch( vty->state )
	{
		
		case VTY_STATE_CONNECTED:
			 
			print_string( "username:" );
			vty->state = VTY_STATE_LOGIN_USERNAME;
			
			break;
		
		case VTY_STATE_LOGIN_USERNAME:
			
			pAccount = VtyMatchUser( cmd );
			
			if( pAccount != NULL )
			{
				print_string( "password:");
				vty->state = VTY_STATE_LOGIN_PASSWORD;
				vty->pAccount = pAccount;
				vty->curShellNode = ( void *)&node_Root;
				break;
			}
			print_string("username error\n\rusername:");
			break;
			
		case VTY_STATE_LOGIN_PASSWORD:
			
			if( !strcmp( cmd, (char *)vty->pAccount->password ) )
			{
				vty->state = VTY_STATE_READY;
				PrintLoginMsg(  );
			}
			else
			{
				print_string( "password error\n\rusername:");
				vty->state = VTY_STATE_LOGIN_USERNAME;
			}
			break;
		
		default:
			break; 
	}

	return ; 
}

int AutoComplete( char *cmdBuf )
{
	int index;
	
	index = strlen( cmdBuf );
	return index;
}

void ShellTask( VTY_STRUCT *vty, INT8 key )
{
	switch(  key  )
	{
		case CTRL_P:
			break;

		case KEY_TABLE:
			vty->cmdIn = AutoComplete( (char *)vty->cmdBuffer );
			break;
					
		case BACKSPACE:
			if( vty->cmdIn > 0 )
			{
				vty->cmdIn--;
				vty->cmdBuffer[vty->cmdIn] = 0;
				print_string("\b \b"); 
			}
			break;

		case LF:
		case ENTER:
			if( vty->state != VTY_STATE_READY ) ShellLogIn( vty, (char *)vty->cmdBuffer );
			else Exec_cmd( vty, (char *)vty->cmdBuffer );
			vty->cmdIn = 0;
			vty->cmdBuffer[vty->cmdIn] = 0;
			break;
		default:
			if( isprint( key ) )
			{
				if( vty->cmdIn < CFG_CBSIZE )
				{
					vty->cmdBuffer[vty->cmdIn++] = key;
					vty->cmdBuffer[vty->cmdIn] = 0;
					if( vty->state == VTY_STATE_LOGIN_PASSWORD )
					{
						print_string( "*");
					}
					else
					{
						print_string( &key );
					}
				}
			}
			break;
		}
}

void ParseCmd( char *cmd, int *argc, char *argv[] )
{
	int i;
	int nPos = 0;
	int nCmdLen = strlen( cmd );
	int a = 0;
		
	/* 取命令 */	
	while( ( cmd[ nPos ] != ' ' ) && ( cmd[ nPos ] != '\0' ) ) nPos++; 
	cmd[ nPos ] = 0;

	i = nPos;

	/* 解析命令行参数 */
	
	while ( i < nCmdLen )
	{
		i += 1;
		while( cmd[i] == ' ' ) i++;    /* 过滤掉多余的空格 */
		
		nPos = i;                       /* 保存参数起始位置 */

		while( cmd[ i ] != ' ' && cmd[ i ] != '\0' ) i++;
		cmd[i] = 0;
		
		argv[a] = &cmd[nPos];
		
		a += 1;
	}
	*argc = a;

}
void Exec_cmd( VTY_STRUCT *vty, char *cmd )
{
	NODE_TABLE *pNode;
	NODE_TABLE *pActiveNode;
	CMD_TABLE  *pCmd;

	char done = 0;

	char buf_print[256] = {0};

	ASSERT( vty != NULL );
	if( vty == NULL ) return;
	
	ASSERT( cmd != NULL );
	if( cmd == NULL ) return;
	
	print_string( "\n\r");
	
	pActiveNode = (NODE_TABLE *)vty->curShellNode;

	if( strlen( cmd ) > 0 )
	{
		StrToLower( cmd );
		ParseCmd( cmd, &vty->argc, vty->argv );

		if( !strcmp( cmd, "quit") || !strcmp( cmd, "q" ) )
		{			
			pNode = pActiveNode->parrent;
			if( pNode != NULL ) pActiveNode = pNode;
		}
		else if( !strcmp( cmd, "exit") )
		{
			vty->curShellNode = ( void *)&node_Root;
		}
		else if( (  !strcmp( cmd,"?" ) || !strcmp( cmd,"help") ) && vty->argc == 0 )
		{
			print_string( "- Command ---------- Description---------------------------\n\r" );
			for( pNode = pActiveNode->child; pNode != NULL; pNode = pNode->next )
			{
				sprintf( buf_print, "%-15s - %s\n\r", pNode->nameText, "command Node" );
				print_string( buf_print );
			}
			for( pCmd = pActiveNode->cmdTable; pCmd != NULL; pCmd = pCmd->next )
			{
				sprintf( buf_print, "%-15s - %s\n\r", pCmd->cmdText, pCmd->cmdDesc );
				print_string( buf_print );
			}
		}
		else if( !strcmp( cmd, "logout" ) )
		{
			print_string( "logout.\n\r");
			vty->state = VTY_STATE_CONNECTED;
		}
		else 
		{
			for( pNode = pActiveNode->child; pNode != NULL; pNode = pNode->next )
			{
				if( !strcmp( cmd, pNode->nameText) )
				{
					if( vty->argc == 0 ) pActiveNode = pNode;
					else print_string( "invalid parameter!\n\r");
					done = 1;
					break;
				}
			}
			if( !done )
			{
				for( pCmd = pActiveNode->cmdTable; pCmd != NULL; pCmd = pCmd->next )
				{
					if( !strcmp( cmd, pCmd->cmdText ) )
					{
						done = 1;
						if( vty->argc == 1 && !strcmp( vty->argv[0],"/?" ) )
						{
							sprintf( buf_print, "usage: %s\n\r", pCmd->cmdUsage );
							print_string( buf_print );
						}
						else if( pCmd->cmdFunc != NULL ) pCmd->cmdFunc( vty ); 
						break;
					}
				}
			}
			if( !done )print_string( "invalid command!\n\r");
		}
	}
	
	if( vty->state == VTY_STATE_READY )
	{
		sprintf( buf_print, "%s>", pActiveNode->nameText );
		print_string( buf_print );
		vty->curShellNode = ( void *)pActiveNode;
	}
	else vty->curShellNode = ( void *)&node_Root;
}

void Shell_process(void)
{
	ConsoleTask();
}

void Shell_init(void)
{
	VtyInit();
	
	VtyAddUser("user","user",VTY_USER);
}

