#include "delay.h"

void delay(uint32_t ms)
{
    for(uint32_t i = 0; i < ms; i++)
    {
        SysTick->VAL = 0;
        while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }
}
