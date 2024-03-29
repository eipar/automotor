#include	<controlador.h>
#include 	<fuzzy.h>

unsigned char code rules[RULE_TOT]={ // fuzzy system rules
// if... and... then...
	T_VLOW,		A_STEADY,	V_OFF,
	T_VLOW,		A_LOW,		V_OFF,
	T_VLOW,		A_MEDIUM,	V_LOW,
	T_VLOW,		A_HIGH,		V_LOW,
	T_VLOW,		A_VHIGH,	V_MEDIUM,
	T_LOW,		A_STEADY,	V_OFF,
	T_LOW,		A_LOW,		V_LOW,
	T_LOW,		A_MEDIUM,	V_LOW,
	T_LOW,		A_HIGH,		V_MEDIUM,
	T_LOW,		A_VHIGH,	V_MEDIUM,
	T_MEDIUM,	A_STEADY,	V_LOW,
	T_MEDIUM,	A_LOW,		V_LOW,
	T_MEDIUM,	A_MEDIUM,	V_MEDIUM,
	T_MEDIUM,	A_HIGH,		V_MEDIUM,
	T_MEDIUM,	A_VHIGH,	V_HIGH,
	T_HIGH,		A_STEADY,	V_MEDIUM,
	T_HIGH,		A_LOW,		V_MEDIUM,
	T_HIGH,		A_MEDIUM,	V_HIGH,
	T_HIGH,		A_HIGH,		V_HIGH,
	T_HIGH,		A_VHIGH,	V_HIGH,
	T_VHIGH,	A_STEADY,	V_MEDIUM,
	T_VHIGH,	A_LOW,		V_HIGH,
	T_VHIGH,	A_MEDIUM,	V_HIGH,
	T_VHIGH,	A_HIGH,		V_VHIGH,
	T_VHIGH,	A_VHIGH,	V_VHIGH,
	T_CRITICAL,	A_STEADY,	V_HIGH,
	T_CRITICAL,	A_LOW, 		V_VHIGH,
	T_CRITICAL,	A_MEDIUM,	V_VHIGH,
	T_CRITICAL,	A_HIGH,		V_MAX,
	T_CRITICAL,	A_VHIGH,	V_MAX
};

unsigned char code input_memf[INPUT_TOT][MF_TOT][4]={
		// input membership functions in point slope form. The first
		// dimension is the input number, the second dimension is
		// the label number and the third dimension refers to the
		// point/slope data
	// membership functions for temperature
	{
		{ 0x00, 0x40, 0x35, 0x12 }, // T_VLOW
		{ 0x2E, 0x12, 0x5D, 0x12 }, // T_LOW
		{ 0x57, 0x14, 0x86, 0x14 }, // T_MEDIUM
		{ 0x7F, 0x14, 0xA1, 0x14 }, // T_HIGH
		{ 0x9A, 0x14, 0xC2, 0x12 }, // T_VHIGH
		{ 0xB5, 0x0D, 0xEE, 0x55 }	// T_CRITICAL
	},
	// membership functions for acceleration
	{
		{ 0x00, 0x80, 0x0F, 0x1A }, // A_STEADY
		{ 0x0C, 0x20, 0x33, 0x15 }, // A_LOW
		{ 0x2D, 0x14, 0x72, 0x0A }, // A_MEDIUM
		{ 0x6B, 0x0A, 0xBF, 0x0A }, // A_HIGH
		{ 0xB7, 0x0C, 0xFC, 0x55 }  // A_VHIGH
	}
};

unsigned char code output_memf[OUTPUT_TOT][MF_TOT]={
		// output membership singletons
		// The first dimension is the output number, the second is
		// the label number
{ V_OFF, V_LOW, V_MEDIUM, V_HIGH, V_VHIGH, V_MAX} 
// motor speed singletons:
// V_OFF, V_LOW, V_MEDIUM, V_HIGH, V_VHIGH,  V_MAX

};

unsigned char outputs[OUTPUT_TOT][MF_TOT], // fuzzy output mu values
			  fuzzy_out[OUTPUT_TOT]; // fuzzy engine outputs
unsigned char input[INPUT_TOT]; // ={ // fuzzified inputs
//				0, 0};

/*****************************************************************
Function: fuzzy_engine
Description: Executes the rules in the fuzzy rule base.
Parameters: none.
Returns: nothing.
Side Effects: none.
*****************************************************************/
unsigned char bdata clause_val; 	// fast access storage for the current clause
sbit operator 	 = clause_val^3; 	// this bit indicates fuzzy operator to use
sbit clause_type = clause_val^7; 	// this bit indicates if the clause is an antecedent or a consequence

void fuzzy_engine(void) {
	bit then; // set true when consequences are being analyzed
	unsigned char 	if_val,  // holds the mu value for antecedents in the current rule
					clause,  // indicates the current clause in the rule base
					mu, 	 // holds the mu value of the current clause
					inp_num, // indicates the input used by the antecedent
					label;   // indicates the membership function used by the antecedent

	then=0; // assume that the first clause is an antecedent
	if_val=MU_MAX; // max out mu for the first rule
	for (clause=0; clause<RULE_TOT; clause++) { // loop through all the rules
		clause_val=rules[clause]; // reads the current clause into bdata
		if (!clause_type) { 	  // if the current clause is an antecedent...
			if (then) { 		  // if a then part was being run...
				then=0; 		  // change back to if
				if_val=MU_MAX; 	  // and reset mu for the rule
			}
			inp_num=clause_val & IO_NUM; 		 // get the referenced input number
			label=(clause_val & LABEL_NUM) / 16; // get the referenced membership function
			mu=compute_memval(inp_num, label);	 // get value of mu for this clause
			if (operator) { // if this is an OR operation...
				if (mu > if_val) { // implement the MAX function
					if_val=mu;
				}
			} else { 			   // if this is an AND operation
				if (mu < if_val) { // implement the MIN function
					if_val=mu;
				}
			}
		} else { // the current clause is a consequence
			then=1; // ensure that the engine knows it is running consequences
			// if the current rule's mu is higher than the referenced
			// output value then store if_val as the new fuzzy output value
			if (outputs[clause_val & IO_NUM][(clause_val & LABEL_NUM) / 16] < if_val) {
				outputs[clause_val & IO_NUM][(clause_val & LABEL_NUM) / 16]=if_val;
			}
		}
	}
	defuzzify(); // compute the fuzzy outputs using the COG method and defuzzify the outputs
}

/*****************************************************************
Function: compute_memval
Description: Calculates mu for a given antecedent assuming that
the membership functions are stored in point slope
format.
Parameters: inp_num - unsigned char. The input number to use.
label - unsigned char. The membership function for
that input to use.
Returns: unsigned char. The computed value for mu.
Side Effects: none.
*****************************************************************/
unsigned char compute_memval(unsigned char inp_num,unsigned char label) {
	int data temp;
	if (input[inp_num] < input_memf[inp_num][label][0]) { // if the input is not under the curve, mu is 0
		return 0;
	} else {
		if (input[inp_num] < input_memf[inp_num][label][2]) { // the input falls under the first vector
			temp=input[inp_num]; // use point-slope math to compute mu
			temp-=input_memf[inp_num][label][0];
			if (!input_memf[inp_num][label][1]) {
				temp=MU_MAX;
			} else {
				temp*=input_memf[inp_num][label][1];
			}
			if (temp <= 0xFF) { // if mu did not exceed 1
				return temp; // return the computed value
			} else {
				return MU_MAX; // make sure that mu stays in range
			}
		} else { // the input falls under the second vector
			temp=input[inp_num]; // use point-slope math to compute mu
			temp-=input_memf[inp_num][label][2];
			temp*=input_memf[inp_num][label][3];
			temp=MU_MAX-temp;
			if (temp < 0) { // make sure that mu is not less than 0
				return 0;
			} else {
				return temp; // mu was positive - return its value
			}
		}
	}
	return 0;
}

/*****************************************************************
Function: defuzzify
Description: Computes the center of gravity of the fuzzy
outputs and calls a function to convert the fuzzy
COGs to outputs usable by the system.
Parameters: none.
Returns: nothing.
Side Effects: The outputs[][] array is cleared.
*****************************************************************/
void defuzzify(void) {
	unsigned long numerator, denominator;
	unsigned char out_num, adj_num;
	for (out_num=0; out_num<OUTPUT_TOT; out_num++) { // for all outputs...
		numerator=0; // reset the summation values
		denominator=0;
		for (adj_num=0; adj_num<MF_TOT; adj_num++) { // compute the summation values
			numerator+=(outputs[out_num][adj_num]*output_memf[out_num][adj_num]);
			denominator+=outputs[out_num][adj_num];
			outputs[out_num][adj_num]=0; // clear the output as its used
		}
		if (denominator) { // make sure that a divide by 0 does not occur
			fuzzy_out[out_num]=numerator/denominator; // finalize the COG
		} else {
			fuzzy_out[out_num]=DEFAULT_VALUE; // no rules fired for this output - set a default value
		}
	}
	normalize(); // change fuzzy output to normal output
}

void normalize (void){

	unsigned char aux;

	//sometimes the output is not in the form 0xX0
	//so we round it
	aux = fuzzy_out[0];
//	P2 = aux;
	if ( (aux&0x0F) >= 0x08 )
	{
		aux = ((aux & 0xF0)+ 0x10);
	}
	else
	{
		aux = aux & 0xF0;
	}
	// we replace de fuzzy output for the PWM values
	P2 = aux; 
	switch (aux)
	{
		case V_OFF:
			fuzzy_out[0] = M_OFF;
			PMW_0_DUTY;
			break;
		case V_LOW:
			fuzzy_out[0] = M_LOW;
			PWM_ON;
			break;
		case V_MEDIUM:
			fuzzy_out[0] = M_MEDIUM;
			PWM_ON;
			break;
		case V_HIGH:
			fuzzy_out[0] = M_HIGH;
			PWM_ON;
			break;
		case V_VHIGH:
			fuzzy_out[0] = M_VHIGH;
			PWM_ON;
			break;
		case V_MAX:
			fuzzy_out[0] = M_MAX;
			PWM_ON;
			break;
		default:
			fuzzy_out[0] = M_LOW;
			PWM_ON;
			break;
	}
}
