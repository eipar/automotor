#define		APLICACION
#include	<controlador.h>

volatile bit check_temp;
volatile bit check_fuzzy;

//extern unsigned char input;
//extern unsigned char fuzzy_out;

void main ( void )
{
	float temp_med;
	char  temp_fuzzy_in, aux;

	PLACA_Init( );

	freno_OFF;
	
	EA = 1;	 // activo las interrupciones

	input[0] =0x00; input[1] = 0x3B;

	
	while( 1 )
	{
		//test de si esto esta vivo o no
		check_board_live();

		if(check_temp)
		{
			check_temp = 0;
			temp_med = LeerTemp( );
			check_status_sensor( temp_med );

			temp_fuzzy_in = (char) temp_med;
			input[0] = temp_fuzzy_in;
		}

		if(check_fuzzy)
		{
		 	check_fuzzy = 0;
			fuzzy_engine();

			aux = fuzzy_out[0];
			PCA0CPH0 = aux;
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
		check_fuzzy = 1;
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

//	if( !ticks_pwm ){
//		ticks_pwm = 100;
//		check_fuzzy = 1;
//	}
}