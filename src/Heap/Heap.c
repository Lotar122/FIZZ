#include "Heap.h"
#include "Global.h"

volatile uint32_t currentHeapPointer = 0;
Block* firstBlock = NULL;
Block* lastBlock = NULL;

void* _heap_base(void)
{
    return (void*)&_heap_start;
}

size_t _heap_size(void)
{
    return (size_t)(&_heap_end - &_heap_start);
}
