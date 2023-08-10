#include "../h/PCB.hpp"

PCB *PCB::running = nullptr;

uint64 PCB::timeSliceCounter = 0;

PCB::~PCB() {
    // probably will need to deallocate system stack
    MemoryAllocator::getInstance()->memFree(stack);
}

PCB *PCB::createPCB(PCB::Body body, void *arg, void *stack) {
//    size_t sizeToAllocate = (sizeof(PCB) / MEM_BLOCK_SIZE + (sizeof(PCB) % MEM_BLOCK_SIZE > 0 ? 1 : 0)) * MEM_BLOCK_SIZE;
//    PCB *pcb = (PCB*) MemoryAllocator::getInstance()->memAlloc(sizeToAllocate);
    PCB *pcb = new PCB(body, arg, stack);
    if (body != nullptr)
        Scheduler::put(pcb);
    else
        setRunning(pcb);
    return pcb;
}

//bool PCB::isFinished() const {
//    return finished;
//}

//void PCB::setFinished(bool finished) {
//    this->finished = finished;
//}

PCB *PCB::getRunning() {
    return running;
}

void PCB::setRunning(PCB *newRunning) {
    running = newRunning;
    running->setState(PCBState::RUNNING);
}

uint64 PCB::getTimeSliceCounter() {
    return timeSliceCounter;
}

void PCB::setTimeSliceCounter(uint64 value) {
    timeSliceCounter = value;
}

void PCB::dispatch() {
    PCB *oldPCB = running;
    if (oldPCB->state != PCBState::FINISHED && oldPCB->state != PCBState::BLOCKED) Scheduler::put(oldPCB);
    setRunning(Scheduler::get());
    contextSwitch(&oldPCB->context, &running->context);
}

void PCB::yield() {
    // syscall thread_dispatch
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x013UL));
    __asm__ volatile ("ecall");
}

PCB::PCBState PCB::getState() const {
    return state;
}

void PCB::setState(PCB::PCBState newState) {
    state = newState;
}

PCB *PCB::getNext() const {
    return next;
}

void PCB::setNext(PCB *newNext) {
    next = newNext;
}

int PCB::getSemWaitRet() const {
    return semWaitRet;
}

void PCB::setSemWaitRet(int ret) {
    semWaitRet = ret;
}

void *PCB::operator new(size_t size) {
    size_t sizeToAllocate = (sizeof(PCB) / MEM_BLOCK_SIZE + (sizeof(PCB) % MEM_BLOCK_SIZE > 0 ? 1 : 0)) * MEM_BLOCK_SIZE;
    return MemoryAllocator::getInstance()->memAlloc(sizeToAllocate);
}

void PCB::operator delete(void *ptr) {
    MemoryAllocator::getInstance()->memFree(ptr);
}

void PCB::threadWrapper() {
    RiscV::popSppSpie();
    running->body(running->arg);
    running->setState(PCBState::FINISHED);
    yield();
}
