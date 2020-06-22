

//SysTick in 1ms
#define INTS_PER_SECOND (1000)

void Set_Systick(int);

void Set_Systick_Int_Time(int);

extern volatile int led_flag;
extern volatile int lin_master_ask_moment;
extern volatile int lin_master_write_moment;

