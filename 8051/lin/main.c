#define		APLICACION
#include	<controlador.h>

volatile bit check_temp;

void main ( void )
{
	PLACA_Init( );
	
	//EA = 1;	 // activo las interrupciones

	led1_OFF;
	led2_OFF;
	led3_OFF;
	led4_OFF;

	while( 1 )
	{
		//test de si esto esta vivo o no
		check_board_live();

	}
}

void timertick ( void ) interrupt 1
{
	static unsigned int ticks_sensores = 1000;

	reset_system_timer ();

	ticks_sensores--;
	
	if( !ticks_sensores )
	{	
		check_temp = 1;
		ticks_sensores = 1000;
	}		 

}


void pca0_handler (void) interrupt 9 
{
	//static unsigned char count_neg_low, count_neg_high, count_pos_low, count_pos_high;
	unsigned char count_pos_low, count_pos_high;
	int total_time;

	CCF1 = 0; //bajo el flag de interrupcion

	led1_ON;

	if ((PCA0CPM1 & 0x10)) //esta configurado en neg edge
	{     //fue neg edge
		  PCA0L     = 0x00;	 //lo reseteo, no me interesa su valor actual
          PCA0H     = 0x00;
		  PCA0CPM1  = 0x21; //pasar a interrupcion a pos edge
	}
	else
	{	  //fue interrupcion de pos edge
		  count_pos_low  = PCA0CPL1;
		  count_pos_high = PCA0CPH1;

		  total_time = (count_pos_high << 8) + count_pos_low;

		  if (total_time > TIME_13BITS) //cuenta llego al valor minimo?
		  {		//prendo la uart
		  		//SCON0 = SCON0 | 0x10;  //prendo el bit 4, recepcion habilitada
				led4_ON;
				led2_OFF;
		  }
		  else
		  {
		  		led4_OFF;//genero algun error del byte largo malo
				led2_ON;
		  }

		  //esto no va aca, sino cuando termino de operar el LIN
		  PCA0CPM1  = 0x11; //pasar a interrupcion a neg edge
	}

}
