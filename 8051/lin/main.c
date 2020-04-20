#define		APLICACION
#include	<controlador.h>

volatile bit check_temp;

void main ( void )
{
	PLACA_Init( );
	
	EA = 1;	 // activo las interrupciones

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


void ext_int_0_handler (void) interrupt 0 
{
	if () //el puerto esta en 0
	{     //fue neg edge
		  //tengo que prender un timer o algo
		  //pasar a interrupcion de pos edge
	}
	else
	{	  //fue interrupcion de pos edge
		  //termino de contar con lo que sea que conte
		  if () //cuenta llego al valor minimo?
		  {		//prendo la uart
		  }
		  else
		  {
		  		//genero algun error del byte largo malo
		  }

	}

}
