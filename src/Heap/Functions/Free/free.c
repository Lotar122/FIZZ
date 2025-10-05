#include "free.h"

#include "Heap/Structures/Block/Block.h"
#include "Heap/Global.h"

void free(void* ptr)
{
	Block* target = NULL;
	Block* block = firstBlock;
	if(!block) return;
	while(!target)
	{
		if(block->data == ptr) { target = block; break; };
		if(!block->next) break;
		block = block->next;
	}

	if(target) target->free = 1;
}
