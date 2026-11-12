#ifndef MEMORY_HEAP_H
#define MEMORY_HEAP_H

#include <common/types.h>

struct MemoryChunk {
    MemoryChunk* next;
    MemoryChunk* prev;
    bool allocated;
    size_t size;
};

class MemoryManager {
protected:
    MemoryChunk* first;
    size_t totalMemory;

public:
    static MemoryManager* activeMemoryManager;

    MemoryManager(size_t start, size_t size);
    ~MemoryManager();

    void* malloc(size_t size);
    void free(void* ptr);

    size_t GetTotalMemory();
    size_t GetUsedMemory();
    size_t GetFreeMemory();
};

void* operator new(size_t size);
void* operator new[](size_t size);
void* operator new(size_t size, void* ptr);
void* operator new[](size_t size, void* ptr);

void operator delete(void* ptr);
void operator delete[](void* ptr);
void operator delete(void* ptr, size_t size);
void operator delete[](void* ptr, size_t size);

#endif