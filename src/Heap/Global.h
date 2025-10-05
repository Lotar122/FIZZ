#pragma once

#include "types.h"

struct Block;

extern volatile uint32_t currentHeapPointer;
extern struct Block* firstBlock;
extern struct Block* lastBlock;

extern uint8_t _heap_start;
extern uint8_t _heap_end;

void* _heap_base(void);

size_t _heap_size(void);
