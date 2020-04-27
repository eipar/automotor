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
	// P0.0  -  TX0 (UART0), Open-Drain, Digital
    // P0.1  -  RX0 (UART0), Open-Drain, Digital
    // P0.2  -  TX1 (UART1), Open-Drain, Digital
    // P0.3  -  RX1 (UART1), Open-Drain, Digital
    // P0.4  -  CEX0 (PCA),  Open-Drain, Digital
    // P0.5  -  CEX1 (PCA),  Open-Drain, Digital
    // P0.6  -  Unassigned,  Open-Drain, Digital
    // P0.7  -  Unassigned,  Open-Drain, Digital

    P74OUT    = 0x08;
    XBR0      = 0x14;
    XBR2      = 0x44;
	
	P0		=	0x00;
	P1		=	0x00;
	P3		=	0x00;
	P4		=	0x00;
	P5		=	0x0F;
	
}

void ADC_Init ( void )
{
	// Seteos para el conversor AD, para la temperatura
	REF0CN    = 0x03; // Uso VREF0, biase=1, sino no anda
	EIE2	=	0x02; // Activada la interrupcion de fin de conversion
	AMX0CF	=	0x00; // Todas entradas singles, ninguna diferencial
	AMX0SL	=	0x02; // Uso AIN0.2
	ADC0CF	=	0x78; // Muestreo a 345,6 KHz(bajar muuuucho), sin ganancia
	ADC0CN	=	0xD0; // ADC habilitado, 
}

void TIMER_Init ( void )
{
	// Seteos para el timer0, el scheduler que salta cada 1 ms
	//tiene cargado 63692
	//va a contar 1843 SYSCLK/12( con SYSCLK de 22.1184MHz me da un cycle machine de 0.542us )
	//y neesito contar 1843 de esos
	//entonces 65535 - 1843 = 63692
	TMOD	=	0x01;	//Modo de 16 bits
	TH0		=	0xF8;  	//valores iniciales
	TL0		=	0xCC;	//valores iniciales
	TR0		=	1;		//prendo timer0
	ET0		=	1;		//prendo la interrupcion del timer
}

void PCA_Init()
{
    PWM_ENABLE; 	  //PCA0CN    = 0x40; //PWN ON
    PCA0MD    = 0x08; //Fuente SYCLK
    //PCA0CPM0  = 0x42; //PWM de 8bits de resolucion
    PWM_VAL_INI;	  //PCA0CPH0  = 0x40; //Valor inicial de 0x40
	PWM_ON;			  //arranca
	PMW_0_DUTY;		  //pero arranca apagado
}

void PLACA_Init ( void )
{
	WDTCN	=	0xDE; //Watchdog timer off
	WDTCN	=	0xAD; //IDEM
	 
	XTAL_Init( );
	PORT_Init( );
	ADC_Init( );
	TIMER_Init( );
	PCA_Init();

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

