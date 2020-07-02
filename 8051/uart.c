#include	<controlador.h>
	
#define MAX_TX	20
#define MAX_RX	20


#define MAX_PALABRA		9

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

void Mensajes (void )
{
	static char inx = 0;
	static unsigned char last_acc = 0;
	unsigned char dato[MAX_REC_BYTES];
	unsigned char gen_checksum;

	for (inx = 0; inx < MAX_REC_BYTES; inx++)
	{
		dato[inx] = PopRx();
	}

	gen_checksum = calculate_checksum(dato[0],dato[1]);

	if ( gen_checksum != dato[2])
	{
		led3_ON; //algo malio sal
	}
	else
	{
		if (dato[0] != last_acc)
		{
			if(led4_STATUS) led4_OFF;
			else led4_ON;
			last_acc = dato[0];
		}
		input[1] = dato[0];
	}

}