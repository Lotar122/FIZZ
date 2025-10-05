#pragma once

#include "types.h"

typedef struct Block
{
	void* data;
	uint32_t size;
	struct Block* next;
	uint8_t free;
} Block;
