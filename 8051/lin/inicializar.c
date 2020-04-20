#include	<controlador.h>

void XTAL_Init(void)
{
	OSCXCN	=	0x67;
	delay( 256 );
	while( !(OSCXCN & 0x80) );
	OSCICN	=	0x88;
	while( !(OSCXCN & 0x80) );
	OSCICN	=	0x08;
}

void PORT_Init(void)
{
    // P0.0  -  TX0 (UART0), Push-Pull,  Digital
    // P0.1  -  RX0 (UART0), Push-Pull,  Digital
    // P0.2  -  CEX0 (PCA),  Push-Pull,  Digital
    // P0.3  -  Unassigned,  Push-Pull,  Digital
    // P0.4  -  Unassigned,  Push-Pull,  Digital
    // P0.5  -  Unassigned,  Push-Pull,  Digital
    // P0.6  -  Unassigned,  Push-Pull,  Digital
    // P0.7  -  Unassigned,  Push-Pull,  Digital
    P0MDOUT   = 0xFF;
    P74OUT    = 0x08;
    XBR0      = 0x0C;
    XBR2      = 0x40;	
 			  
	P74OUT	=	0x08;	//P5H en Push-Pull
	
	
	
}

void TIMER_Init ( void )
{
	// Seteos para el timer0, el scheduler que salta cada 1 ms
	//tiene cargado 63692
	//va a contar 1843 SYSCLK/12( con SYSCLK de 22.1184MHz me da un cycle machine de 0.542us )
	//y necesito contar 1843 de esos
	//entonces 65535 - 1843 = 63692
	TMOD	=	0x01;	//Modo de 16 bits
	TH0		=	0xF8;  	//valores iniciales
	TL0		=	0xCC;	//valores iniciales
	TR0		=	1;		//prendo timer0
	ET0		=	1;		//prendo la interrupcion del timer
}

void PLACA_Init ( void )
{
	WDTCN	=	0xDE; //Watchdog timer off
	WDTCN	=	0xAD; //IDEM
	 
	XTAL_Init( );
	PORT_Init( );
	TIMER_Init( );
}

//------------------------------------------------------------------------------
// Nombre de la función:	delay( unsigned int tiempo )
// Proposito:				Retardar la ejecución del programa tantos usegundos
//							como se indique
// Parametros:				tiempo: retardo en usegundos
// Retorna:					void							       
//------------------------------------------------------------------------------
void		delay				( unsigned int tiempo )
{
	for( tiempo; tiempo > 0; tiempo-- )
		_nop_( );
}

