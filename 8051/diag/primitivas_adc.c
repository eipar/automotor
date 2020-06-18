#include	<ntcm12.h>

code long Cuentas[]=
{4095,3782,3581,3297,2929,2505,2061,1678,1283,978,746,562,427,325,249,194,151,120,95,50,0,0};
code int Temperatura[] = {-50,-40,-30,-20,-10,0,10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,150}; 

float LeerTemp ( void )
{
	float Temp;
	long parcial_cuentas;
	char i;
	long auxp;

    ADC0CN = 0x00; // Desactivar la conversion

	Temp = 0;
	parcial_cuentas = 0;

	for ( auxp = i = 0 ; i < MUESTRAS ; i++ )
	{
		auxp = auxp + buff_temp[ i ];
	} 

	parcial_cuentas = auxp / MUESTRAS;

	Temp = TablaNTC( parcial_cuentas );

	ADC0CN = 0xD0;

	return Temp;
}

float TablaNTC (long cuenta)
{
	int i, dif_cuentas,dif_cuentas_intervalo, dif_temp;
	float T, division, multi;

	if (cuenta < 50) return 140;

	for ( i = 0 ; i < INTERVALO ; i++)
	{
		if( cuenta > Cuentas[i])
			break;
	}
	/* vamos a separar esto en partes:
		T = (Temperatura[i+1] - ((int)(Cuentas[i+1] - cuenta) * (( Temperatura[i+1] - Temperatura[i]) / (int)(Cuentas[i+1] - Cuentas[i]))));
		
		dif_cuentas = (int)(Cuentas[i+1] - cuenta);
		dif_temp = ( Temperatura[i+1] - Temperatura[i]);
		division = dif_temp / dif_cuentas;
		multi = dif_cuentas * division;
		T = Temperatura[i+1] - multi;  
	 */
//	T = (Temperatura[i+1] - ((int)(Cuentas[i+1] - cuenta) * (( Temperatura[i+1] - Temperatura[i]) / (int)(Cuentas[i+1] - Cuentas[i]))));

		dif_cuentas = (int)(Cuentas[i+1] - cuenta);
		dif_cuentas_intervalo = (int)(Cuentas[i+1] - Cuentas[i]);
		dif_temp = ( Temperatura[i+1] - Temperatura[i]);
		division = ((float) dif_temp) / ((float) dif_cuentas_intervalo);
		multi = ((float)dif_cuentas) * division;
		T = ((float)Temperatura[i+1]) - multi;  	


	return T;	
}