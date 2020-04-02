#include	<controlador.h>

void reset_system_timer ( void )
{
 	TR0		=	0;
	TH0		=	0xF8;  
	TL0		=	0xCC + TL0;
	TR0		=	1;
}

void check_status_sensor( float temp_med )
{
	if(temp_med>70)//supongamos que por aca arrancamos el motor
	{ led1_ON;} else { led1_OFF; }
	if (temp_med<(-45))//corto contra VCC en el sensor, alarma
	{ led2_ON; } else { led2_OFF; }
	if (temp_med>=140)//corto contra GND en el sensor, alarma
	{ led3_ON; } else { led3_OFF; }
}

void check_board_live ( void )
{
	if(tact3_STATUS) led4_ON;
	else led4_OFF;
}