#define		APLICACION
#include	<controlador.h>

volatile bit check_temp;

void main ( void )
{
	float temp_med;

	PLACA_Init( );

	freno_OFF;
	
	EA = 1;	 // activo las interrupciones
	
	while( 1 )
	{
		//test de si esto esta vivo o no
		check_board_live();

		if(check_temp)
		{
			check_temp = 0;
			temp_med = LeerTemp( );
			check_status_sensor( temp_med );
		}

	}
}

void timertick ( void ) interrupt 1
{
	static unsigned int ticks_sensores = 1000;
	static unsigned int ticks_gpio = 100;
	static unsigned int ticks_pwm = 100;

	reset_system_timer ();

	ticks_sensores--;
	ticks_gpio--;
	ticks_pwm--;
	
	if( !ticks_sensores )
	{	
		check_temp = 1;
		ticks_sensores = 1000;
	}		 

//	//El Duty del PWM varia automaricamente, para mostrar que funciona el medio puente H
//	if( !ticks_pwm ){
//		ticks_pwm = 100;
//		if(PWM_MAX_STATUS){
//			PWM_VAL_INI;
//		}else{
//			PWM_UP;
//		}
//	}

	//El Duty del PWM varia con los botoncitos del kit
	if( !ticks_pwm ){
		ticks_pwm = 100;
		if(PWM_MAX_STATUS){
			PWM_VAL_INI;
		}else{
			if(tact0_STATUS) 
			{PMW_0_DUTY;}
			else if (tact1_STATUS)
			{PWM_SPEED_DOWN;}
			else if (tact2_STATUS)
			{PWM_SPEED_UP;}
			else if (tact3_STATUS) 
			{PWM_ON;}
		}
	}

}