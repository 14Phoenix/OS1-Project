#ifndef PROJECT_BASE_H_MEMORYALLOCATOR
#define PROJECT_BASE_H_MEMORYALLOCATOR


#include "../lib/hw.h"


class MemoryAllocator {
public:
    void* memAlloc(size_t size);

    int memFree(void*);

    static MemoryAllocator* getInstance();

    MemoryAllocator(const MemoryAllocator& memoryAllocator) = delete;
    MemoryAllocator(MemoryAllocator&& memoryAllocator) = delete;
    MemoryAllocator& operator=(const MemoryAllocator& memoryAllocator) = delete;
    MemoryAllocator& operator=(MemoryAllocator&& memoryAllocator) = delete;

private:
    MemoryAllocator();

    struct BlockHeader {
        BlockHeader *next;
        size_t size;
    };

    BlockHeader *freeMemHead = nullptr;

};


#endif //PROJECT_BASE_H_MEMORYALLOCATOR