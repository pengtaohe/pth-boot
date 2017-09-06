#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"

#include "shell.h"
#include "vty.h"

static VTY_STRUCT VtyData[MAX_VTY];
static VTY_STRUCT *pVtyFirst = NULL;
static VTY_ACCOUNT VtyAccount[MAX_ACCOUNT];


static UINT8 print_port_type = (UINT8)COM_PORT;
INT8 ConsoleReadChar( void );

UINT8 get_print_port_type( void )
{
    return print_port_type;
}

VTY_STRUCT *GetFirstVty( void )
{
    return pVtyFirst;
}

VTY_STRUCT *NewVty( void )
{
    UINT16 i;
    for( i = 0; i < MAX_VTY; i++ )
    {
        if( VtyData[i].state == VTY_STATE_FREE )
        {
            return ( VTY_STRUCT *)&VtyData[i];
        }
    }
    return NULL;
}

void FreeVty( VTY_STRUCT *pVty )
{
	pVty->state =  VTY_STATE_FREE;
} 

void InitVtyData( VTY_STRUCT *vty, VTY_TYPE vtyType )
{
    vty->vtyType = vtyType;
    vty->cmdIn = 0;
    vty->pAccount = NULL;
    vty->next = NULL;
    vty->prev = NULL;
    vty->state = VTY_STATE_CONNECTED;
}

VTY_STRUCT *CreateVty( VTY_TYPE vtyType )
{
    VTY_STRUCT *pVty;
    VTY_STRUCT *pTempVty;

    pVty = NewVty();
    if( pVty == NULL ) return NULL;

    InitVtyData( pVty, vtyType );

    if( pVtyFirst == NULL )pVtyFirst = pVty;
    else
    {
        for( pTempVty = pVtyFirst; pTempVty->next != NULL; pTempVty = pTempVty->next );
        pTempVty->next = pVty;
        pVty->prev = pTempVty;
    }

    return pVty;
}

UINT8 VtyAddUser(const char *user, const char *pwd, VTY_PROPERTY pro)
{
	UINT8 i, rel = 0;
	for(i = 0; i < MAX_ACCOUNT; i ++)
	{
		if(ACCOUNT_FREE == VtyAccount[i].flag)
		{
			strcpy((char*)VtyAccount[i].username, user);
			strcpy((char*)VtyAccount[i].password , pwd);
			VtyAccount[i].property = pro;
			VtyAccount[i].flag = ACCOUNT_USED;
			rel = 1;
		} 
	}
	return rel;
}

VTY_ACCOUNT * VtyMatchUser( char *username )
{
    int i;

    for( i = 0; i < MAX_ACCOUNT; i++ )
    {
        if( ACCOUNT_USED == VtyAccount[i].flag )
        {
            if( !strcmp( username, (char *)VtyAccount[i].username ) ) 
				return (VTY_ACCOUNT *)&VtyAccount[i];
        }
    }
    return NULL;
}

VTY_STATUS DeleteVty( VTY_STRUCT *pVty )
{
	VTY_STRUCT *vtyPrev, *vtyNext;

	vtyPrev = pVty->prev ;
	vtyNext = pVty->next;
	if( vtyPrev != NULL ) vtyPrev->next = vtyNext;
	if( vtyNext != NULL ) vtyNext->prev = vtyPrev;

	pVty->state = VTY_STATE_FREE;

	return VTY_OK;
}

/* receive character from uart, port for stm32f407  ,hept, 2017.8.30 */
INT8 ConsoleReadChar( void )
{
	register INT8 key = 0; 
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        key = (UINT8)USART_ReceiveData(USART1);
        print_port_type = COM_PORT;
    }

    return key;
}

int ConsoleTask( void )
{
    VTY_STRUCT *vty;
    INT8 key;

    for( vty = GetFirstVty( ); vty != NULL && vty->vtyType != VTY_CONSOLE; vty = vty->next );

    key = ConsoleReadChar( );

    if( key != 0 )
    {
        if( vty == NULL )
        {
            if( key == ENTER )
            {
                vty = CreateVty( VTY_CONSOLE );
                if( vty == NULL ) return VTY_ERROR;
            }
            else
            {
                return VTY_OK;
            }
        }

        ShellTask( vty, key );
    }
    
    return VTY_OK;
}

void VtyInit(void)
{
	memset(VtyData, 0, sizeof(VtyData));
	memset(VtyAccount, 0, sizeof(VtyAccount));
}

