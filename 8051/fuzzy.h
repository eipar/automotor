
#define INPUT_TOT		0x02
#define MF_TOT			0x06
#define	RULE_TOT		90
#define OUTPUT_TOT		0x01
#define MU_MAX			0xFF
#define IO_NUM 			0x07
#define LABEL_NUM 		0x70
#define DEFAULT_VALUE	0xE0

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
										  										 
//define the rules for the consequences
#define  V_OFF		0x80
#define  V_LOW		0x90
#define  V_MEDIUM	0xA0
#define  V_HIGH		0xB0
#define  V_VHIGH	0xC0
#define  V_MAX		0xD0

//define the values off PWM related to the putput singletons
#define  M_OFF		0xFF
#define  M_LOW		0xCC
#define  M_MEDIUM	0x99
#define  M_HIGH		0x66
#define  M_VHIGH	0x33
#define  M_MAX		0x00 //0x06

extern unsigned char outputs[OUTPUT_TOT][MF_TOT], // fuzzy output mu values
			  		 fuzzy_out[OUTPUT_TOT]; // fuzzy engine outputs
extern unsigned char input[INPUT_TOT]; // fuzzified inputs
