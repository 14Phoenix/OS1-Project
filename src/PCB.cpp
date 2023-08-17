#include "../h/PCB.hpp"

PCB *PCB::running = nullptr;

PCB *PCB::oldRunning = nullptr;

PCB *PCB::waitHead = nullptr;

uint64 PCB::timeSliceCounter = 0;

PCB::~PCB() {
    // probably will need to deallocate system stack
    MemoryAllocator::getInstance()->memFree(stack);
}

PCB *PCB::createPCB(PCB::Body body, void *arg, void *stack) {
    PCB *pcb = new PCB(body, arg, stack);
    if (body != nullptr)
        Scheduler::put(pcb);
    else
        setRunning(pcb);
    return pcb;
}

PCB *PCB::createPCBnoStart(PCB::Body body, void *arg, void *stack) {
    PCB *pcb = new PCB(body, arg, stack);
    return pcb;
}

PCB *PCB::createKernelPCB(PCB::Body body, void *arg, void *stack) {
    PCB *pcb = new PCB(body, arg, stack, true);
    if (body != nullptr)
        Scheduler::put(pcb);
    else
        setRunning(pcb);
    return pcb;
}

PCB *PCB::getRunning() {
    return running;
}

void PCB::setRunning(PCB *newRunning) {
    running = newRunning;
    running->setState(PCBState::RUNNING);
}

time_t PCB::getTimeSliceCounter() {
    return timeSliceCounter;
}

void PCB::setTimeSliceCounter(time_t value) {
    timeSliceCounter = value;
}

void PCB::incTimeSliceCounter() {
    timeSliceCounter++;
}

void PCB::dispatch() {
    oldRunning = running;
    if (oldRunning->state != PCBState::FINISHED &&
        oldRunning->state != PCBState::BLOCKED &&
        oldRunning->state != PCBState::WAITING &&
        oldRunning->state != PCBState::SLEEPING) Scheduler::put(oldRunning);
    setRunning(Scheduler::get());
    contextSwitch(&oldRunning->context, &running->context);
    freeStackOldRunning();
}

void PCB::yield() {
    // syscall thread_dispatch
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x013UL));
    __asm__ volatile ("ecall");
}

void PCB::join(PCB *pcb) {
    if (pcb->state == PCBState::FINISHED) return;
    running->setState(PCBState::WAITING);
    pcb->joinQueue.put(running);
    dispatch();
}

void PCB::wait(time_t time) {
    running->setState(PCBState::SLEEPING);
    running->setTimeToSleep(time);

    PCB *current = waitHead, *previous = nullptr;
    while (current != nullptr && current->timeToSleep <= running->timeToSleep) {
        running->timeToSleep -= current->timeToSleep;
        previous = current;
        current = current->next;
    }

    if (previous != nullptr)
        previous->next = running;
    else
        waitHead = running;
    running->setNext(current);

    if (current != nullptr) current->timeToSleep -= running->timeToSleep;

    dispatch();
}

void PCB::updateWait() {
    if (waitHead != nullptr) waitHead->timeToSleep--;
    while (waitHead != nullptr && waitHead->timeToSleep == 0) {
        PCB *pcb = waitHead->next;
        Scheduler::put(waitHead);
        waitHead = pcb;
    }
}

PCB::PCBState PCB::getState() const {
    return state;
}

void PCB::setState(PCB::PCBState newState) {
    if (newState == PCBState::FINISHED) {
        for (PCB *pcb = joinQueue.get(); pcb != nullptr; pcb = joinQueue.get())
            Scheduler::put(pcb);
    }
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

time_t PCB::getTimeToSleep() const {
    return timeToSleep;
}

void PCB::setTimeToSleep(time_t time) {
    timeToSleep = time;
}

time_t PCB::getTimeSlice() const {
    return timeSlice;
}

bool PCB::isKernelPCB() const {
    return kernelPCB;
}

void *PCB::operator new(size_t size) {
    size_t sizeToAllocate = (sizeof(PCB) / MEM_BLOCK_SIZE + (sizeof(PCB) % MEM_BLOCK_SIZE > 0 ? 1 : 0)) * MEM_BLOCK_SIZE;
    return MemoryAllocator::getInstance()->memAlloc(sizeToAllocate);
}

void PCB::operator delete(void *ptr) {
    MemoryAllocator::getInstance()->memFree(ptr);
}

void PCB::threadWrapper() {
    freeStackOldRunning();
    if (running->isKernelPCB())
        RiscV::ms_sstatus(RiscV::BitMaskSstatus::SSTATUS_SPP);
    else
        RiscV::mc_sstatus(RiscV::BitMaskSstatus::SSTATUS_SPP);
    RiscV::popSppSpie();
    running->body(running->arg);
    running->setState(PCBState::FINISHED);
    yield();
}

void PCB::freeStackOldRunning() {
    if (oldRunning != nullptr && oldRunning->state == PCBState::FINISHED) {
        MemoryAllocator::getInstance()->memFree(oldRunning->stack);
    }
}
