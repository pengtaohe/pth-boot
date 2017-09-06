#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "vty.h"

typedef struct _CMD_TABLE
{
	struct _CMD_TABLE *prev;
	struct _CMD_TABLE *next;
	char *cmdText;                             /* √¸¡Ó√˚≥∆ */
	void (*cmdFunc)(  VTY_STRUCT *vty ) ; /* √¸¡Ó∫Ø ˝÷∏’Î */	
	char *cmdDesc;                             /* √¸¡Ó√Ë ˆ */
	char *cmdUsage;                            /* ”√∑®     */
}CMD_TABLE;

typedef struct _NODE_TABLE
{
	struct _NODE_TABLE *child;
	struct _NODE_TABLE *parrent;
	struct _NODE_TABLE *prev;
	struct _NODE_TABLE *next;
	CMD_TABLE *cmdTable;
	char *nameText;
}NODE_TABLE;

#define DefShellCmd( cmdName, cmdText, cmdFunc,cmdDesc, cmdUsage, cmdPrev, cmdNext ) \
	void cmdFunc(  VTY_STRUCT *vty ) ; \
	CMD_TABLE cmdName = { cmdPrev, cmdNext, cmdText, cmdFunc,cmdDesc,cmdUsage }; \
	void cmdFunc(  VTY_STRUCT *vty ) 

void ShellTask( VTY_STRUCT *vty, INT8 key );
void Shell_process(void);
void Shell_init(void);
void PrintLoginMsg( void );

#endif
