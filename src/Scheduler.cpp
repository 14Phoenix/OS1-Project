#include "../h/Scheduler.hpp"
#include "../h/PCB.hpp"
#include "../h/MemoryAllocator.hpp"

Scheduler::Scheduler() {
    size_t sizeToAllocate = (DEFAULT_STACK_SIZE / MEM_BLOCK_SIZE + (DEFAULT_STACK_SIZE % MEM_BLOCK_SIZE > 0 ? 1 : 0)) * MEM_BLOCK_SIZE;
    void *stack = MemoryAllocator::getInstance()->memAlloc(sizeToAllocate);
    emptyPCB = PCB::createPCBnoStart(emptyBody, nullptr, stack);
}

PCB *Scheduler::get() {
    PCB *pcb = getInstance()->queue.get();
    if (pcb == nullptr) pcb = getInstance()->emptyPCB;
    return pcb;
}

void Scheduler::put(PCB *pcb) {
    if (pcb == getInstance()->emptyPCB) return;
    pcb->setState(PCB::PCBState::READY);
    getInstance()->queue.put(pcb);
}

Scheduler *Scheduler::getInstance() {
    static Scheduler instance;
    return &instance;
}

void Scheduler::freeEmptyPCB() {
    MemoryAllocator::getInstance()->memFree(getInstance()->emptyPCB);
}

void Scheduler::emptyBody(void *arg) {
    while (true) {

    }
}
