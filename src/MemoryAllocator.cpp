#include "../h/MemoryAllocator.hpp"

MemoryAllocator::MemoryAllocator() {
    size_t availableMem = (size_t)((uint8*) HEAP_END_ADDR - (uint8*) HEAP_START_ADDR);

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
                BlockHeader *newBlock = (BlockHeader*)((uint8*) current + sizeof(BlockHeader) + size);
                newBlock->next = current->next;
                newBlock->size = remaining - sizeof(BlockHeader);
                current->size = size;
                if (previous != nullptr)
                    previous->next = newBlock;
                else
                    freeMemHead = newBlock;
            } else {
                if (previous != nullptr)
                    previous->next = current->next;
                else
                    freeMemHead = current->next;
            }

            current->next = nullptr;
            allocated = (void*)((uint8*) current + sizeof(BlockHeader));
            break;
        }
        previous = current;
        current = current->next;
    }

    return allocated;
}

int MemoryAllocator::memFree(void *ptr) {
    if ((uint8*) ptr >= (uint8*) HEAP_END_ADDR || (uint8*) ptr < (uint8*) HEAP_START_ADDR) return -1;


    BlockHeader *block = (BlockHeader*)((uint8*) ptr - sizeof(BlockHeader));
    if (block->next != nullptr) return -2;

    BlockHeader *current = freeMemHead, *previous = nullptr;
    while (current != nullptr && (uint8*) current < (uint8*) block) {
        previous = current;
        current = current->next;
    }

    if (previous != nullptr)
        previous->next = block;
    else
        freeMemHead = block;
    block->next = current;

    BlockHeader *startBlockToJoin = block;

    // Try to join previous and block
    if (previous != nullptr && (uint8*) previous + sizeof(BlockHeader) + previous->size == (uint8*) block) {
        startBlockToJoin = previous;
        previous->next = block->next;
        previous->size += sizeof(BlockHeader) + block->size;
    }

    // Try to join previous + block or just block (if previous == nullptr) and current
    if (current != nullptr && (uint8*) startBlockToJoin + sizeof(BlockHeader) + startBlockToJoin->size == (uint8*) current) {
        startBlockToJoin->next = current->next;
        startBlockToJoin->size += sizeof(BlockHeader) + current->size;
    }

    return 0;
}

MemoryAllocator *MemoryAllocator::getInstance() {
    static MemoryAllocator instance;
    return &instance;
}
