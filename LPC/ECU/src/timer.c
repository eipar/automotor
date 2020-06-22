#include "timer.h"

extern volatile uint8_t zero_long_sent;

void TIMER0_IRQHandler(void)
{
	if(Chip_TIMER_MatchPending(LPC_TIMER0, 1))
	{
		Chip_TIMER_ClearMatch(LPC_TIMER0, 1);
		zero_long_sent = 1;
		Chip_TIMER_Disable(LPC_TIMER0);
	}
}

void TIMER0_Init(void)
{
	uint32_t timerFreq;

	Chip_TIMER_Init(LPC_TIMER0);
	timerFreq = Chip_Clock_GetSystemClockRate();
	Chip_TIMER_Reset(LPC_TIMER0);
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 1);
	Chip_TIMER_SetMatch(LPC_TIMER0, 1, (timerFreq / TICKRATE_HZ1)); //CAMBIAR
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 1);

	NVIC_ClearPendingIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER0_IRQn);

}



