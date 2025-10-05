#include "Functions/SysTickInit/SysTickInit.h"

uint32_t SystemCoreClock = HSI_FREQ;

void SysTickInit(uint32_t ticks)
{
    SysTick->LOAD  = ticks - 1;   // counts down from this value
    SysTick->VAL   = 0;           // clear current value
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}
