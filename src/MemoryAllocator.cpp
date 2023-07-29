#include "../h/MemoryAllocator.hpp"

MemoryAllocator::MemoryAllocator() {
    size_t availableMem = ((uint64) HEAP_END_ADDR) - ((uint64) HEAP_START_ADDR);

    if (availableMem >= sizeof(BlockHeader) + MEM_BLOCK_SIZE) {
        freeMemHead = (BlockHeader*) HEAP_START_ADDR;
        freeMemHead->next = nullptr;
        freeMemHead->size = availableMem - sizeof(BlockHeader);
    }
}

void *MemoryAllocator::memAlloc(size_t size) {
    void* allocated = nullptr;

    BlockHeader *current = freeMemHead, *previous = nullptr;
    while (current != nullptr) {
        if (size <= current->size) {
            size_t remaining = current->size - size;

            if (remaining >= sizeof(BlockHeader) + MEM_BLOCK_SIZE) {
                BlockHeader *newBlock = (BlockHeader*)(((uint8*) current) + sizeof(BlockHeader) + size);
                newBlock->next = current->next;
                newBlock->size = remaining - sizeof(BlockHeader);
                current->size = size;
                if (previous != nullptr) previous->next = newBlock;
                else freeMemHead = newBlock;
            } else {
                if (previous != nullptr) previous->next = current->next;
                else freeMemHead = current->next;
            }

            current->next = nullptr;
            allocated = (void*)((uint8*)current + sizeof(BlockHeader));
            break;
        }
        previous = current;
        current = current->next;
    }

    return allocated;
}

int MemoryAllocator::memFree(void *ptr) {
    if ((uint8*) ptr >= (uint8*) HEAP_END_ADDR || (uint8*) ptr < (uint8*) HEAP_START_ADDR) return -1;

    BlockHeader *allocatedBlock = (BlockHeader*)(((uint8*) ptr) - sizeof(BlockHeader));
    if (allocatedBlock->next != nullptr) return -2;

    BlockHeader *current = freeMemHead, *previous = nullptr;
    while (current != nullptr && current < allocatedBlock) {



        previous = current;
        current = current->next;
    }





    return 0;
}

MemoryAllocator *MemoryAllocator::getInstance() {
    static MemoryAllocator instance;
    return &instance;
}
