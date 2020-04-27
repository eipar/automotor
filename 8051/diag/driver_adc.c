#define		DRIVER_NTCM12
#include	<ntcm12.h>

void SensTemp ( void ) interrupt 15
{
	static unsigned char inx = 0;

	ADC0CN = 0x00; //interrumpio, lo apago

	buff_temp[ inx++ ] = (int) ADC0; //mando la muestra al buffer

	inx %= MUESTRAS;

	AMX0SL = 0x02;

	ADC0CN = 0xD0; //lo prendo y lo mando a convertir de nuevo
}