#include "bumpPointerMalloc.h"

#include "Heap/Global.h"

void* bumpPointerMalloc(uint32_t size)
{
	if(currentHeapPointer + size > _heap_size()) { return NULL; };
	void* res = _heap_base() + currentHeapPointer;
	currentHeapPointer += size;
	return res;
}
