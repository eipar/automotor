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
    // P0.2  -  TX1 (UART1), Push-Pull,  Digital
    // P0.3  -  RX1 (UART1), Push-Pull,  Digital
    // P0.4  -  CEX0 (PCA),  Push-Pull,  Digital
    // P0.5  -  CEX1 (PCA),  Push-Pull,  Digital
    // P0.6  -  Unassigned,  Push-Pull,  Digital
    // P0.7  -  Unassigned,  Push-Pull,  Digital

    //P0MDOUT   = 0xFF;
    P74OUT    = 0x08;
    XBR0      = 0x14;
    XBR2      = 0x44;
  
	P74OUT	=	0x08;	//P5H en Push-Pull
	
	P0		=	0x00;
	P3		=	0x00;
	P4		=	0xFF;
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

	//ahora la parte de seteo para el timer1 de la uart 0
	CKCON     = 0x10;   		// el timer1 usa el SYSCLK de base
    TMOD      = TMOD | 0x20;	// el timer1 en modo 8bit con auto reload
	TR1		=	1;				// prendo timer1
    TH1       = 0xB8;			// valor para 9600bps

	//dejo el timer2 listo para empezar a contar cuando necesite la demora del LIN
    RCAP2H    = 0xE0;
    TH2       = 0xE0;
}

void PCA_Init( void )
{
    PWM_ENABLE; 	  //PCA0CN    = 0x40; //PWN ON
    PCA0MD    = 0x08; //Fuente SYCLK/4
    //PCA0CPM0  = 0x42; //PWM de 8bits de resolucion
    PWM_VAL_INI;	  //PCA0CPH0  = 0x40; //Valor inicial de 0x40
	PWM_ON;			  //arranca
	PMW_0_DUTY;		  //pero arranca apagado
	PCA0CPM1  = 0x11;   // configuro el modulo 1 en neg edge y activo la interrupcion
}

void UART_Init()
{
    //SCON0     = 0x50;	 // modo 1, receptor prendido
	SCON1		= 0x40;	 // UART1, modo 1, receptor apagado por ahora 
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
	UART_Init();

    IE        = 0x92;	//activo las interrupciones
    EIE1      = 0x08;	//activo las interrupciones del PCA
	EIE2      = 0x40;	//activo las interrupciones de la UART
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

