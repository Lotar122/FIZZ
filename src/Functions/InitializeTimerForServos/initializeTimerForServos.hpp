#pragma once

extern "C" {
#include "types.h"
}

void initializeTimer3ForServos(uint16_t initialStateS1 = 1500, uint16_t initialStateS2 = 1500, uint16_t initialStateS3 = 1500, uint16_t initialStateS4 = 1500);