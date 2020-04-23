#include	<controlador.h>
	
#define MAX_TX	9
#define MAX_RX	11


#define MAX_PALABRA		10

volatile unsigned char BUF_TX[MAX_TX];
volatile unsigned char BUF_RX[MAX_RX];
volatile unsigned char Rx_In,Rx_Out;
volatile unsigned char Tx_In,Tx_Out;
volatile unsigned char flagTx;

void PushTx(unsigned char dato)
{
	BUF_TX[Tx_In] = dato;
	Tx_In ++;
	Tx_In %= MAX_TX;	
}

void PushRx(unsigned char dato)
{
	BUF_RX[Rx_In] = dato;
	Rx_In ++;
	Rx_In %= MAX_RX;	
}

int PopRx (void)
{
	int dato = -1;

	if( Rx_In != Rx_Out )
	{
		dato = 	(int ) BUF_RX[Rx_Out];
		Rx_Out ++;
		Rx_Out %= MAX_RX;
	}

	return dato;
}

int PopTx (void)
{
	int dato = -1;

	if( Tx_In != Tx_Out )
	{
		dato = 	(int ) BUF_TX[Tx_Out];
		Tx_Out ++;
		Tx_Out %= MAX_TX;
	}

	return dato;
}

//void Transmitir( char *msg)
//{
//	char i;
//
//	for (i = 0 ; msg[i] ; i++)
//		PushTx( msg[i]);
//
//	if ( !flagTx )
//	{
//		flagTx = 1;	
//		ARRANCAR_TX;
//	}
//}

void Mensajes (void )
{
	static char inx = 0;
	char dato[MAX_REC_BYTES];

	for (inx = 0; inx < MAX_REC_BYTES; inx++)
	{
		dato[inx] = PopRx();
	}
}