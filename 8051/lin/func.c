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


unsigned char calculate_checksum( unsigned char byte0, unsigned char byte1 )
{
	unsigned char suma;
	suma = ~(byte0+byte1);

	return suma;
}

//unsigned char calculate_checksum_send( unsigned char cant_bytes, unsigned char * datos )
//{
//	unsigned char suma, inx;
//	for (inx=0;inx < cant_bytes; inx++)
//	{
//		suma = suma + datos[inx];
//	}
//	suma = ~suma;
//	return suma;
//}