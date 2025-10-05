#include "malloc.h"

#include "Heap/Global.h"
#include "Heap/Structures/Block/Block.h"
#include "Heap/Functions/BumpPointerMalloc/bumpPointerMalloc.h"

void* malloc(uint32_t size)
{
	if(currentHeapPointer + size > _heap_size()) { return NULL; }

	//search in free list
	Block* feasibleBlock = NULL;
	if(firstBlock)
	{
		Block* block = firstBlock;
		while(!feasibleBlock)
		{
			if(block->free && block->size >= size) { feasibleBlock = block; break; };
			if(!block->next) break;
			block = block->next;
		}
	}

	if(feasibleBlock)
	{
		feasibleBlock->free = 0;
		return feasibleBlock->data;
	}
	else
	{
		Block* block = NULL;
		if(!firstBlock)
		{
			firstBlock = (Block*)bumpPointerMalloc(sizeof(Block));
			lastBlock = firstBlock;
			block = firstBlock;
		}
		else
		{
			block = (Block*)bumpPointerMalloc(sizeof(Block));
			lastBlock->next = block;
			lastBlock = block;
		}

		block->next = NULL;
		block->free = 0;

		block->size = size;
		block->data = _heap_base() + currentHeapPointer;

		currentHeapPointer += size;
		return block->data;
	}

	return NULL;
}