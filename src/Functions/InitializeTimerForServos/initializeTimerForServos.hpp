#pragma once

#include "types.h"

extern "C" {
#include "stm32l476xx.h"
}

void initializeTimer3ForServos(uint16_t initialStateS1 = 1500, uint16_t initialStateS2 = 1500, uint16_t initialStateS3 = 1500, uint16_t initialStateS4 = 1500);