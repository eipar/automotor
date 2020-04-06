#include	<controlador.h>
#include 	<fuzzy.h>

/*****************************************************************
Function: fuzzy_engine
Description: Executes the rules in the fuzzy rule base.
Parameters: none.
Returns: nothing.
Side Effects: none.
*****************************************************************/
unsigned char bdata clause_val; // fast access storage for the current clause
sbit operator = clause_val^3; // this bit indicates fuzzy operator to use
sbit clause_type = clause_val^7; // this bit indicates if the clause is an antecedent or a consequence

void fuzzy_engine(void) {
	bit then; // set true when consequences are being analyzed
	unsigned char 	if_val, // holds the mu value for antecedents in the current rule
					clause, // indicates the current clause in the rule base
					mu, // holds the mu value of the current clause
					inp_num, // indicates the input used by the antecedent
					label; // indicates the membership function used by the antecedent

	then=0; // assume that the first clause is an antecedent
	if_val=MU_MAX; // max out mu for the first rule
	for (clause=0; clause<RULE_TOT; clause++) { // loop through all the rules
		clause_val=rules[clause]; // reads the current clause into bdata
		if (!clause_type) { // if the current clause is an antecedent...
			if (then) { // if a then part was being run...
				then=0; // change back to if
				if_val=MU_MAX; // and reset mu for the rule
			}
			inp_num=clause_val & IO_NUM; // get the referenced input number
			label=(clause_val & LABEL_NUM) / 16; // get the referenced membership function
			mu=compute_memval(inp_num, label);// get value of mu for this clause
			if (operator) { // if this is an OR operation...
				if (mu > if_val) { // implement the MAX function
					if_val=mu;
				}
			} else { // if this is an AND operation
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
			if (temp < 0x100) { // if mu did not exceed 1
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
	unsigned char i, j;
	for (i=0; i<OUTPUT_TOT; i++) { // for all outputs...
		numerator=0; // reset the summation values
		denominator=0;
		for (j=0; j<MF_TOT; j++) { // compute the summation values
			numerator+=(outputs[i][j]*output_memf[i][j]);
			denominator+=outputs[i][j];
			outputs[i][j]=0; // clear the output as its used
		}
		if (denominator) { // make sure that a divide by 0 does not occur
			fuzzy_out[i]=numerator/denominator; // finalize the COG
		} else {
			fuzzy_out[i]=DEFAULT_VALUE; // no rules fired for this output - set a default value
		}
	}
	normalize(); // change fuzzy output to normal output
}



