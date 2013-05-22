#ifndef MEMORY_H
#define MEMORY_H

#include <new>
#include "Heap.h"

namespace CGE
{
    void* allocate(size_t inSize);
    void* allocate(size_t inSize, Heap& inHeap);
    void* reallocate(void* inMemory, size_t inSize, Heap& inHeap);
    void release(void* inMemory);
    size_t addReference(void* inMemory);
    size_t removeReference(void* inMemory);
}

extern void* operator new(size_t inSize) throw(std::bad_alloc);
extern void operator delete(void* inData) throw();

#endif
