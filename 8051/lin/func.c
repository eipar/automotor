#include	<controlador.h>

void reset_system_timer ( void )
{
 	TR0		=	0;
	TH0		=	0xF8;  
	TL0		=	0xCC + TL0;
	TR0		=	1;
}

void check_board_live ( void )
{
	if(tact3_STATUS) led4_ON;
	else led4_OFF;
}