#ifndef __VTY_H__
#define __VTY_H__

/* 类型定义 */
typedef unsigned char UINT8;
typedef char           INT8;
typedef unsigned short int  UINT16;
typedef short int     INT16;
typedef unsigned long UINT32;
typedef long          INT32;

#undef NULL
#define NULL ( void * )0


/* 虚拟终端状态 */
typedef enum
{
	VTY_STATE_FREE  = 0,                /* 空闲状态 */
	VTY_STATE_CONNECTED,                /* 连接状态 */
	VTY_STATE_LOGIN_USERNAME,          /* 正在登录并输入username */
	VTY_STATE_LOGIN_PASSWORD,          /* 正在登录并输入password */
	VTY_STATE_READY                     /* 登录成功 */
}VTY_STATE;  

/* 虚拟终端连接类型 */
typedef enum
{
	VTY_UNKNOW   = 0,  /* 未知 */
	VTY_CONSOLE  = 1,  /* 控制台连接 */
	VTY_TELNET   = 2   /* TELNET连接 */
}VTY_TYPE;

/* 用户权限 */
typedef enum
{
	VTY_USER,
	VTY_ADMIN,
	VTY_DEBUG
}VTY_PROPERTY;

#define MAX_LEN_USERNAME 16
#define MAX_LEN_PASSWORD 16

#define CONSOLE 1
#define TELNET 2

/* 帐户状态 */
typedef enum
{
	ACCOUNT_FREE = 0,
	ACCOUNT_USED = 1
}ACCOUNT_STATE;


typedef struct _VTY_ACCOUNT
{
	UINT8        username[MAX_LEN_USERNAME];
	UINT8        password[MAX_LEN_PASSWORD];
	VTY_PROPERTY property;
	ACCOUNT_STATE flag;
}VTY_ACCOUNT;

#define VTY_IO_BUFFER_LEN  64
#define VTY_CMD_BUFFER_LEN 64
#define MAX_PARAM 10        /* 定义最多参数个数 */
typedef struct _VTY_STRUCT
{
	VTY_STATE      state;
	VTY_TYPE       vtyType;
	VTY_ACCOUNT    *pAccount; 
#if 0	
	UINT8          vtyOutBuf[VTY_IO_BUFFER_LEN];
	UINT8          vtyInBuf[VTY_IO_BUFFER_LEN];
#endif	
	UINT8          cmdBuffer[VTY_CMD_BUFFER_LEN];
	UINT8          cmdIn;
#if 0	
	UINT8 		   clientSocket;
#endif
	void           *curShellNode;
	int            argc;
	char           *argv[MAX_PARAM];
	struct _VTY_STRUCT  *prev;
	struct _VTY_STRUCT  *next; 
}VTY_STRUCT;

typedef enum
{
	VTY_IN,
	VTY_OUT
}VTY_IO_TYPE;

/* 虚拟终端输入/输出消息 */
typedef struct _VTY_IO_MSG
{
	VTY_IO_TYPE  type;   /* 消息类型 */
	VTY_STRUCT   *pVty;  /* 虚拟终端结构指针 */
	UINT16 size;
	char  vtyIoBuffer[VTY_IO_BUFFER_LEN];
}VTY_IO_MSG;

#define VTY_MSG_LEN sizeof( VTY_IO_MSG )

#define MAX_VTY 1          /* 最大终端数 */
#define MAX_ACCOUNT 3 /* 最大帐户数 */

#define VTY_PRI     200
#define VTY_TASK_STACK 8192


#define ENTER 0x0d
#define LF 0x0a
#define BACKSPACE 0x8
#define KEY_TABLE  0x09
#define SPACE 0x20
#define ESC 0x1b

#define CTRL_K     0x0B
#define CTRL_P     0x10
#define CTRL_S     0x13
#define KEY_UP     0x41
#define KEY_DOWN   0x42
#define KEY_LEFT   0x43
#define KEY_RIGHT  0x44

typedef enum
{
	VTY_OK = 0,
	VTY_ERROR
}VTY_STATUS;

typedef enum
{
	COM_PORT,
	TELNET_PORT
}DEV_PRINT_PORT;

void VtyInit(void);

void InitVtyData( VTY_STRUCT *vty, VTY_TYPE vtyType );

UINT8 VtyAddUser(const char *user, const char *pwd, VTY_PROPERTY pro);
VTY_ACCOUNT *VtyMatchUser( char *username );

#define vty_printf printf
#define debug_printf printf

VTY_STRUCT *CreateVty( VTY_TYPE vtyType );
VTY_STRUCT *GetFirstVty( void );

#ifndef ASSERT

#ifdef _DEBUG_
#define ASSERT( val ) \
{ \
	if( !(val) ) \
	{ \
		debug_printf("Error: %s, line %d\n\r",__FILE__,__LINE__); \
	} \
}
#else
#define ASSERT( val ) {}
#endif

#endif

int ConsoleTask( void );
UINT8 get_print_port_type( void );

#endif
