

void fuzzy_engine(void);
unsigned char compute_memval(unsigned char,unsigned char );
void defuzzify(void);

#define INPUT_TOT		0x02
#define MF_TOT			0x06
#define	RULE_TOT		90
#define OUTPUT_TOT		0x01
#define MU_MAX			0xFF
#define IO_NUM 			0x07
#define LABEL_NUM 		0x70
#define DEFAULT_VALUE	0xF0

// define constants for the temperature input
#define T_VLOW 		0x00
#define T_LOW		0x10
#define T_MEDIUM	0x20
#define T_HIGH		0x30
#define T_VHIGH		0x40
#define T_CRITICAL 	0x50

// define constants for the acceleration input
#define A_STEADY 	0x01
#define A_LOW 		0x11
#define A_MEDIUM 	0x21
#define A_HIGH 		0x31
#define A_VHIGH		0x41
										  										 
//define the values off PWM related to the putput singletons
#define  V_OFF		0xFF
#define  V_LOW		0xF0
#define  V_MEDIUM	0x80
#define  V_HIGH		0x40
#define  V_VHIGH	0x10
#define  V_MAX		0x06


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
		{ 0x00, 0x00, 0x70, 0x11 }, // T_VLOW
		{ 0x70, 0x11, 0x8E, 0x11 }, // T_LOW
		{ 0x8E, 0x11, 0xAC, 0x11 }, // T_MEDIUM
		{ 0xAC, 0x11, 0xBB, 0x11 }, // T_HIGH
		{ 0xBB, 0x11, 0xCA, 0x11 }, // T_VHIGH
		{ 0xCA, 0x11, 0xFF, 0x00 }	// T_CRITICAL
	},
	// membership functions for acceleration
	{
		{ 0x00, 0x00, 0x00, 0x0A }, // A_STEADY
		{ 0x00, 0x15, 0x19, 0x14 }, // A_LOW
		{ 0x19, 0x0A, 0x72, 0x0A }, // A_MEDIUM
		{ 0x72, 0x0A, 0xBF, 0x0A }, // A_HIGH
		{ 0xBF, 0x0A, 0xFF, 0x0 }  // A_VHIGH
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

unsigned char outputs[MF_TOT], // fuzzy output mu values
			  fuzzy_out; // fuzzy engine outputs
unsigned char input[INPUT_TOT] ={ // fuzzified inputs
				0, 0};