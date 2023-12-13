#include "../lib/hw.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/RiscV.hpp"
#include "../h/PCB.hpp"
#include "../h/K_Semaphore.hpp"
#include "../h/syscall_c.hpp"
#include "../h/syscall_cpp.hpp"


extern void userMain();

void userMainWrapperBody(void *arg) {
    userMain();
}

int main() {
    RiscV::w_stvec((uint64) &RiscV::trapVector | 0x01UL);

    PCB *mainPCB = PCB::createKernelPCB(nullptr, nullptr, nullptr);
    if (mainPCB == nullptr) return 0;

    void *stack = MemoryAllocator::getInstance()->memAlloc(
            (DEFAULT_STACK_SIZE / MEM_BLOCK_SIZE + (DEFAULT_STACK_SIZE % MEM_BLOCK_SIZE > 0 ? 1 : 0)) * MEM_BLOCK_SIZE);
    PCB *consoleOutputPCB = PCB::createKernelPCB(&RiscV::consoleOutputBody, nullptr, stack);
    if (consoleOutputPCB == nullptr) return 0;

    void *userStack = MemoryAllocator::getInstance()->memAlloc(
            (DEFAULT_STACK_SIZE / MEM_BLOCK_SIZE + (DEFAULT_STACK_SIZE % MEM_BLOCK_SIZE > 0 ? 1 : 0)) * MEM_BLOCK_SIZE);
    PCB *userMainPCB = PCB::createPCB(&userMainWrapperBody, nullptr, userStack);

    RiscV::ms_sstatus(RiscV::BitMaskSstatus::SSTATUS_SIE);

    thread_join(userMainPCB);

    // Empty output buffer
    while (!CharOutputBuffer::getInstance()->isEmpty()) {
        thread_dispatch();
    }

    RiscV::mc_sstatus(RiscV::BitMaskSstatus::SSTATUS_SIE);

    CharOutputBuffer::getInstance()->freeSem();
    CharInputBuffer::getInstance()->freeSem();

    Scheduler::freeEmptyPCB();

    return 0;
}