#include "../h/K_Semaphore.hpp"
#include "../h/PCB.hpp"
#include "../h/Scheduler.hpp"
#include "../h/MemoryAllocator.hpp"

K_Semaphore *K_Semaphore::createK_Semaphore(int value) {
    K_Semaphore *semaphore = new K_Semaphore(value);
    return semaphore;
}

int K_Semaphore::deleteK_Semaphore(K_Semaphore *semaphore) {
    if (semaphore == nullptr) return -1;
    for (PCB *blockedPCB = semaphore->queue.get(); blockedPCB != nullptr; blockedPCB = semaphore->queue.get()) {
        blockedPCB->setSemWaitRet(-2);
        Scheduler::put(blockedPCB);
    }
    return MemoryAllocator::getInstance()->memFree(semaphore);
}

void K_Semaphore::wait() {
    if (--value < 0) block();
}

void K_Semaphore::signal() {
    if (++value <= 0) unblock();
}

void K_Semaphore::block() {
    PCB *oldPCB = PCB::getRunning();
    oldPCB->setState(PCB::PCBState::BLOCKED);
    queue.put(oldPCB);
    PCB::dispatch();
}

void K_Semaphore::unblock() {
    PCB *blockedPCB = queue.get();
    Scheduler::put(blockedPCB);
}

void *K_Semaphore::operator new(size_t size) {
    size_t sizeToAllocate = (sizeof(K_Semaphore) / MEM_BLOCK_SIZE + (sizeof(K_Semaphore) % MEM_BLOCK_SIZE > 0 ? 1 : 0)) * MEM_BLOCK_SIZE;
    return MemoryAllocator::getInstance()->memAlloc(sizeToAllocate);
}

void K_Semaphore::operator delete(void *ptr) {
    MemoryAllocator::getInstance()->memFree(ptr);
}

K_Semaphore::~K_Semaphore() {
    for (PCB *blockedPCB = queue.get(); blockedPCB != nullptr; blockedPCB = queue.get()) {
        blockedPCB->setSemWaitRet(-2);
        Scheduler::put(blockedPCB);
    }
}
