#include "../h/RiscV.hpp"

void RiscV::popSppSpie() {
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void RiscV::consoleOutputBody(void *arg) {
    uint8 *consoleStatus = (uint8*) CONSOLE_STATUS;
    uint8 *consoleTransfer = (uint8*) CONSOLE_TX_DATA;

    while (true) {
        while (*consoleStatus & CONSOLE_TX_STATUS_BIT) {
            char character = CharOutputBuffer::getInstance()->getc();
            *consoleTransfer = character;
        }
    }
}

void RiscV::handleSupervisorTrap() {
    uint64 volatile syscallCode;
    uint64 volatile arg1;
    uint64 volatile arg2;
    uint64 volatile arg3;
    uint64 volatile arg4;
    uint64 volatile stackPointer;

    __asm__ volatile ("mv %[a0], a0;"
                      "mv %[a1], a1;"
                      "mv %[a2], a2;"
                      "mv %[a3], a3;"
                      "mv %[a4], a4;"
                      "mv %[a5], a5;"
                      : [a0] "=r" (syscallCode)
                      , [a1] "=r" (arg1)
                      , [a2] "=r" (arg2)
                      , [a3] "=r" (arg3)
                      , [a4] "=r" (arg4)
                      , [a5] "=r" (stackPointer));

    uint64 scause = r_scause();

    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {
        uint64 volatile sepc = r_sepc();
        uint64 volatile sstatus = r_sstatus();

        switch (syscallCode) {
            // void* mem_alloc(size_t size)
            case 0x01UL: {
                uint64 volatile numOfBlocks = (uint64) arg1;
                void *allocated = MemoryAllocator::getInstance()->memAlloc(numOfBlocks * MEM_BLOCK_SIZE);
                __asm__ volatile ("mv t1, %[stackPointer];"
                                  "sd %[ptr], 10 * 8(t1)" : : [stackPointer] "r" (stackPointer), [ptr] "r" (allocated) : "t1");
                break;
            }

            // int mem_free(void *ptr)
            case 0x02UL: {
                void* volatile ptr = (void*) arg1;
                int errorCode = MemoryAllocator::getInstance()->memFree(ptr);
                __asm__ volatile ("mv t1, %[stackPointer];"
                                  "sd %[error], 10 * 8(t1)" : : [stackPointer] "r" (stackPointer), [error] "r" (errorCode) : "t1");
                break;
            }

            // int thread_create(thread_t *handle, void(*start_routine)(void*), void* arg, void* stack_space)
            case 0x011UL: {
                PCB **handle = (PCB**) arg1;
                PCB::Body startRoutine = (PCB::Body) arg2;
                void *arguments = (void*) arg3;
                void *stackSpace = (void*) arg4;

                *handle = PCB::createPCB(startRoutine, arguments, stackSpace);
                int errorCode = 0;
                if (*handle == nullptr) errorCode = -1;

                __asm__ volatile ("mv t1, %[stackPointer];"
                                  "sd %[error], 10 * 8(t1)" : : [stackPointer] "r" (stackPointer), [error] "r" (errorCode) : "t1");
                break;
            }

            // int thread_exit()
            case 0x012UL: {
                PCB::getRunning()->setState(PCB::PCBState::FINISHED);
                PCB::setTimeSliceCounter(0);
                PCB::dispatch();
                int errorCode = 0;
                __asm__ volatile ("mv t1, %[stackPointer];"
                                  "sd %[error], 10 * 8(t1)" : : [stackPointer] "r" (stackPointer), [error] "r" (errorCode) : "t1");
                break;
            }

            // void thread_dispatch()
            case 0x013UL: {
                PCB::setTimeSliceCounter(0);
                PCB::dispatch();
                break;
            }

            // void thread_join(thread_t handle)
            case 0x014UL: {
                PCB *handle = (PCB*) arg1;
                PCB::join(handle);
                break;
            }

            // int sem_open(sem_t *handle, unsigned init)
            case 0x021UL: {
                K_Semaphore **handle = (K_Semaphore**) arg1;
                unsigned init = (unsigned) arg2;

                *handle = K_Semaphore::createK_Semaphore(init);
                int errorCode = 0;
                if (*handle == nullptr) errorCode = -1;

                __asm__ volatile ("mv t1, %[stackPointer];"
                                  "sd %[error], 10 * 8(t1)" : : [stackPointer] "r" (stackPointer), [error] "r" (errorCode) : "t1");
                break;
            }

            // int sem_close(sem_t handle)
            case 0x022UL: {
                K_Semaphore *handle = (K_Semaphore*) arg1;
                int errorCode = K_Semaphore::deleteK_Semaphore(handle);
                __asm__ volatile ("mv t1, %[stackPointer];"
                                  "sd %[error], 10 * 8(t1)" : : [stackPointer] "r" (stackPointer), [error] "r" (errorCode) : "t1");
                break;
            }

            // int sem_wait(sem_t id)
            case 0x023UL: {
                K_Semaphore *id = (K_Semaphore *) arg1;
                int errorCode = 0;
                if (id == nullptr) {
                    errorCode = -1;
                } else {
                    PCB::getRunning()->setSemWaitRet(0);
                    id->wait();
                    errorCode = PCB::getRunning()->getSemWaitRet();
                }
                __asm__ volatile ("mv t1, %[stackPointer];"
                                  "sd %[error], 10 * 8(t1)" : : [stackPointer] "r"(stackPointer), [error] "r"(errorCode) : "t1");
                break;
            }

            // int sem_signal(sem_t id)
            case 0x024UL: {
                K_Semaphore *id = (K_Semaphore*) arg1;
                int errorCode = 0;
                if (id == nullptr)
                    errorCode = -1;
                else
                    id->signal();
                __asm__ volatile ("mv t1, %[stackPointer];"
                                  "sd %[error], 10 * 8(t1)" : : [stackPointer] "r"(stackPointer), [error] "r"(errorCode) : "t1");
                break;
            }

            // int time_sleep(time_t time)
            case 0x031UL: {
                time_t time = (time_t) arg1;
                PCB::wait(time);
                int errorCode = 0;
                __asm__ volatile ("mv t1, %[stackPointer];"
                                  "sd %[error], 10 * 8(t1)" : : [stackPointer] "r"(stackPointer), [error] "r"(errorCode) : "t1");
                break;
            }

            // char getc()
            case 0x041UL: {
                char character = CharInputBuffer::getInstance()->getc();
                __asm__ volatile ("mv t1, %[stackPointer];"
                                  "sd %[c], 10 * 8(t1)" : : [stackPointer] "r"(stackPointer), [c] "r"(character) : "t1");
                break;
            }

            // void putc(char c)
            case 0x042UL: {
                char character = (char) arg1;
                CharOutputBuffer::getInstance()->putc(character);
                break;
            }

            default:
                break;
        }

        mc_sip(BitMaskSip::SIP_SSIP);
        w_sepc(sepc + 4);
        w_sstatus(sstatus);

    } else if (scause == 0x8000000000000001UL) {
        // Timer interrupt
        mc_sip(BitMaskSip::SIP_SSIP);

        PCB::updateWait();
        PCB::incTimeSliceCounter();

        if (PCB::getTimeSliceCounter() >= PCB::getRunning()->getTimeSlice()) {
            uint64 volatile sepc = r_sepc();
            uint64 volatile sstatus = r_sstatus();
            PCB::setTimeSliceCounter(0);
            PCB::dispatch();
            w_sepc(sepc);
            w_sstatus(sstatus);
        }
    } else if (scause == 0x8000000000000009UL) {
        // Console interrupt
        int interruptNumber = plic_claim();

        uint8 *consoleStatus = (uint8*) CONSOLE_STATUS;
        uint8 *consoleRecieve = (uint8*) CONSOLE_RX_DATA;

        if (interruptNumber == CONSOLE_IRQ) {
            while (*consoleStatus & CONSOLE_RX_STATUS_BIT && !CharInputBuffer::getInstance()->isFull()) {
                uint8 character = *consoleRecieve;
                CharInputBuffer::getInstance()->putc(character);
            }
        }

        plic_complete(interruptNumber);

    } else {
        // default
    }

}
