#pragma once

#include "types.h"

#include "stm32l476xx.h"

#define HSI_FREQ 16000000UL

extern uint32_t SystemCoreClock;

void SysTickInit(uint32_t);
