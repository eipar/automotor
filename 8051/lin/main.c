#define		APLICACION
#include	<controlador.h>

volatile bit check_temp;

volatile bit flag_msg;
volatile unsigned char Temp, Diag;
extern unsigned char flagTx;
extern unsigned char Rx_In;

void main ( void )
{
	PLACA_Init( );

	led1_OFF;
	led2_OFF;
	led3_OFF;
	led4_OFF;

	Temp = 0x55; Diag = 0xAA;
		  
	while( 1 )
	{
		//test de si esto esta vivo o no
		check_board_live();

		Temp = P4;

		if(flag_msg == 1)
		{
		 	flag_msg = 0;
			Mensajes();
		}

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
	unsigned char count_pos_low, count_pos_high;
	int total_time;

	CCF1 = 0; //bajo el flag de interrupcion

	if ((PCA0CPM1 & 0x10)) //esta configurado en neg edge
	{     //fue neg edge
		  PCA0L     = 0x00;	 //lo reseteo, no me interesa su valor actual
          PCA0H     = 0x00;
		  PCA_ON_POSEDGE; //PCA0CPM1  = 0x21; //pasar a interrupcion a pos edge
	}else{	  //fue interrupcion de pos edge
		  count_pos_low  = PCA0CPL1;
		  count_pos_high = PCA0CPH1;
		  total_time = (count_pos_high << 8) + count_pos_low;
		  if (total_time > TIME_13BITS) //cuenta llego al valor minimo?
		  {		
				PCA_INT_OFF; //EIE1 = EIE1 & 0xF7;//desactivar la interrupcion del modulo
				//prendo la uart
		  		SCON1 = SCON1 | 0x10;  //prendo el bit 4, recepcion habilitada
		  } else {
				led2_ON;
		  }
	}

}

void uart0_handler (void) interrupt 20
{
	unsigned char dato;
	int dato1;
	unsigned char checksum;

	//MaqEst para decodificar
	static char estado = ESPERA;
	static unsigned char Rx_count;

	if ( RX )
	{
		APAGAR_RX;
		dato = BUFER_RX_TX;
		if  (dato == 0x55 && estado == ESPERA)
		{ 
			estado = SYNC_BREAK_OK;
		}
		else if (estado == SYNC_BREAK_OK)
		{
			if (dato == ID_REC_DATA)
			{
				estado = REC_DATA; //me van a mandar el dato
			}
			else if (dato == ID_SEND_DATA)
			{
				estado = ESPERA; //tengo que mandar yo el dato y terminar
				PushTx(Temp);
				PushTx(Diag);
				checksum = ~(Temp+Diag);
				PushTx(checksum);
				SCON1 = SCON1 & 0xEF;  //apago el bit 4, recepcion deshabilitada
				T2CON     = 0x04; 	   //prendo el timer 2
				ET2 = 1;		  	   //prendo su interrupcion
			}
			else
			{
				estado = ESPERA; //NO_ID;	//no es para mi
				//volver a activar el edge detector
				PCA_ON_NEGEDGE; //configuro el pca en neg edge
				PCA_INT_ON;		//prendo su interrupcion
				SCON1 = SCON1 & 0xEF;  //apago el bit 4, recepcion deshabilitada
			}	
		}
		else if (estado == REC_DATA)
		{
			PushRx(dato);
			Rx_count++;
			if (Rx_count == MAX_REC_BYTES)
			{
				Rx_count = 0x00;
				flag_msg = 1;
				estado = ESPERA;
				//volver a activar el edge detector
				PCA_ON_NEGEDGE; //configuro el pca en neg edge
				PCA_INT_ON;		//prendo su interrupcion
				SCON1 = SCON1 & 0xEF;  //apago el bit 4, recepcion deshabilitada
			}
		}
	}

	if (TX)
	{
		APAGAR_TX;
		dato1 = PopTx();
		if ( dato1 != -1 )
			BUFER_RX_TX	=  (unsigned char)dato1;
		else
			flagTx = 0;
			estado = ESPERA;
			PCA_ON_NEGEDGE;  //configuro el pca en neg edge
			PCA_INT_ON;      //prendo su interrupcion
		    SCON1 = SCON1 & 0xEF;  //apago el bit 4, recepcion deshabilitada
	}
}

void timer2_handler (void) interrupt 5
{
	// esto es para el tiempo que tiene que pasar entre ID y dato
	TF2 	  = 0;    //bajo el flag de interrupcion
	T2CON     = 0x00; //apago el timer 2
	ET2 = 0;		  //apago su interrupcion
	if ( !flagTx ) //si no esta prendido el transmisor, lo prendo
	{
		flagTx = 1;	
		ARRANCAR_TX;
	}
}
