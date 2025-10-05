#pragma once

#include "stm32l476xx.h"
#include "types.h"

void servoSetAngle(int, TIM_TypeDef*, uint8_t);