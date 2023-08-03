#include "../h/PCB.hpp"

PCB *PCB::running = nullptr;

uint64 PCB::timeSliceCounter = 0;

PCB *PCB::createPCB(PCB::Body body, void *arg, void *stack) {
//    size_t sizeToAllocate = (sizeof(PCB) / MEM_BLOCK_SIZE + (sizeof(PCB) % MEM_BLOCK_SIZE > 0 ? 1 : 0)) * MEM_BLOCK_SIZE;
//    PCB *pcb = (PCB*) MemoryAllocator::getInstance()->memAlloc(sizeToAllocate);
    PCB *pcb = new PCB(body, arg, stack);
    return pcb;
}

bool PCB::isFinished() const {
    return finished;
}

void PCB::setFinished(bool finished) {
    this->finished = finished;
}

void PCB::yield() {
    // add a0 code for dispatch for handleSupervisorTrap
    // __asm__ volatile ("ecall");
}

void *PCB::operator new(size_t size) {
    size_t sizeToAllocate = (sizeof(PCB) / MEM_BLOCK_SIZE + (sizeof(PCB) % MEM_BLOCK_SIZE > 0 ? 1 : 0)) * MEM_BLOCK_SIZE;
    return MemoryAllocator::getInstance()->memAlloc(sizeToAllocate);
}

void PCB::operator delete(void *ptr) {
    MemoryAllocator::getInstance()->memFree(ptr);
}

void PCB::threadWrapper() {
    // sret here
    running->body(running->arg);
    running->setFinished(true);
    yield();
}

void PCB::dispatch() {
    PCB *oldPCB = running;
    if (!oldPCB->isFinished()) Scheduler::put(oldPCB);
    running = Scheduler::get();
    contextSwitch(&oldPCB->context, &running->context);
}
