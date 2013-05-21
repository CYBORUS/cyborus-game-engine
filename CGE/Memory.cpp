#include "Memory.h"
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>

namespace CGE
{
    static Heap GeneralHeap("General");
    static const unsigned int Canary = 0xdeadbeef;

    struct Header
    {
        Heap* heap;
        Header* next;
        Header* previous;
        size_t references;
        size_t size;
        unsigned int canary;
    };

    static Header* gFirstHeader = NULL;

    static Header* getHeader(void* inMemory)
    {
        Header* outHeader = (Header*)((char*)inMemory - sizeof(Header));
        assert(outHeader->canary == Canary);
        return outHeader;
    }

    void* allocate(size_t inSize)
    {
        return allocate(inSize, GeneralHeap);
    }

    void* allocate(size_t inSize, Heap& inHeap)
    {
        size_t total = sizeof(Header) + inSize;
        char* memory = (char*)malloc(total);
        char* outBlock = NULL;

        if (memory)
        {
            inHeap.allocate(inSize);

            Header* header = (Header*)memory;
            memset(header, 0, sizeof(Header));
            header->heap = &inHeap;
            header->size = inSize;
            header->canary = Canary;

            if (gFirstHeader)
            {
                assert(gFirstHeader->previous == NULL);
                gFirstHeader->previous = header;
                header->next = gFirstHeader;
            }

            gFirstHeader = header;
            outBlock = memory + sizeof(Header);
        }

        return outBlock;
    }

    void* reallocate(void* inMemory, size_t inSize, Heap& inHeap)
    {
        assert(inSize > 0);

        void* outBlock = inMemory;

        if (inMemory)
        {
            Header* header = getHeader(inMemory);

            if (header->size < inSize)
            {
                outBlock = allocate(inSize, inHeap);
                memcpy(outBlock, inMemory, header->size);
                release(inMemory);
            }
        }
        else
        {
            outBlock = allocate(inSize, inHeap);
        }

        return outBlock;
    }

    void release(void* inMemory)
    {
        if (inMemory)
        {
            Header* header = getHeader(inMemory);
            assert(header->references < 1);

            header->heap->release(header->size);

            if (gFirstHeader == header) gFirstHeader = gFirstHeader->next;

            if (header->next) header->next->previous = header->previous;
            if (header->previous) header->previous->next = header->next;

            free(header);
        }
    }

    size_t addReference(void* inMemory)
    {
        size_t outReferences = 0;

        if (inMemory)
        {
            Header* header = getHeader(inMemory);
            outReferences = ++header->references;
        }

        return outReferences;
    }

    size_t removeReference(void* inMemory)
    {
        size_t outReferences = 0;

        if (inMemory)
        {
            Header* header = getHeader(inMemory);
            assert(header->references > 0);
            outReferences = --header->references;
        }

        return outReferences;
    }
}

void* operator new(size_t inSize) throw(std::bad_alloc)
{
    return CGE::allocate(inSize, CGE::GeneralHeap);
}

void operator delete(void* inData) throw()
{
    CGE::release(inData);
}
