#include "Clock.h"

volatile uint32_t m_SysTick;
volatile uint32_t TimingDelay;

static void TimingDelay_Decrement(void);

void SysClock_Init(void)
{
	SysTick_Config(SystemCoreClock / 1000);
	NVIC_EnableIRQ(SysTick_IRQn);
}

uint32_t SysTick_GetTick(void)
{
	return m_SysTick;
}


void SysTick_Delay(__IO uint32_t target_ms)
{
 TimingDelay = target_ms;
 
  while(TimingDelay != 0)
	{
		;
	}		
}

void SysTick_Delay2(uint32_t target_ms)
{
	uint32_t current_ms = 0;
	current_ms = SysTick_GetTick();
	while(SysTick_GetTick() - current_ms < target_ms)
	{
	}
}

void SysTick_Handler(void)
{
	m_SysTick++;
	TimingDelay_Decrement();
}


static void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}