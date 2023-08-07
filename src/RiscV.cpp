#include "../h/RiscV.hpp"

#include "../lib/console.h"

void RiscV::popSppSpie() {
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void RiscV::handleSupervisorTrap() {
    uint64 syscallCode;
    uint64 arg1;
    uint64 arg2;
    uint64 arg3;
    uint64 arg4;

    __asm__ volatile ("mv %0, a0" : "=r" (syscallCode));
    __asm__ volatile ("mv %0, a1" : "=r" (arg1));
    __asm__ volatile ("mv %0, a2" : "=r" (arg2));
    __asm__ volatile ("mv %0, a3" : "=r" (arg3));
    __asm__ volatile ("mv %0, a4" : "=r" (arg4));

    uint64 scause = r_scause();

    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {
        uint64 volatile sepc = r_sepc();
        uint64 volatile sstatus = r_sstatus();

        switch (syscallCode) {
            // void* mem_alloc(size_t size)
            case 0x01UL: {
                uint64 volatile numOfBlocks = (uint64) arg1;
//                __asm__ volatile ("mv %[size], a1" : [size] "=r" (numOfBlocks));
                void *allocated = MemoryAllocator::getInstance()->memAlloc(numOfBlocks * MEM_BLOCK_SIZE);
                __asm__ volatile ("mv a0, %[ptr]" : : [ptr] "r" ((uint64) allocated));
                break;
            }

            // int mem_free(void *ptr)
            case 0x02UL: {
                void* volatile ptr = (void*) arg1;
//                __asm__ volatile ("mv %[p], a1" : [p] "=r" (ptr));
                int errorCode = MemoryAllocator::getInstance()->memFree(ptr);
                __asm__ volatile ("mv a0, %[error]" : : [error] "r" ((uint64) errorCode));
                break;
            }

            // int thread_create(thread_t *handle, void(*start_routine)(void*), void* arg, void* stack_space)
            case 0x011UL: {
//                PCB** handle;
//                __asm__ volatile ("mv %[handle], a1" : [handle] "=r" (handle));
//                PCB::Body startRoutine;
//                __asm__ volatile ("mv %[routine], a2" : [routine] "=r" (startRoutine));
//                void* arguments;
//                __asm__ volatile ("mv %[arg], a3" : [arg] "=r" (arguments));
//                void* stackSpace;
//                __asm__ volatile ("mv %[stack], a4" : [stack] "=r" (stackSpace));

                PCB **handle = (PCB**) arg1;
                PCB::Body startRoutine = (PCB::Body) arg2;
                void *arguments = (void*) arg3;
                void *stackSpace = (void*) arg4;

                *handle = PCB::createPCB(startRoutine, arguments, stackSpace);
                int errorCode = 0;
                if (*handle == nullptr) errorCode = -1;

                __asm__ volatile ("mv a0, %[code]" : : [code] "r" ((uint64) errorCode));
                break;
            }

            // int thread_exit()
            case 0x012UL: {
                PCB::running->finished = true;
                PCB::timeSliceCounter = 0;

                // if the thread is finished should i delete the thread, if yes then i can put else in PCB::dispatch

                break;
            }

            // void thread_dispatch()
            case 0x013UL: {
                PCB::timeSliceCounter = 0;
                PCB::dispatch();
                break;
            }

            // void thread_join(thread_t handle)
            case 0x014UL:
                break;

            // int sem_open(sem_t *handle, unsigned init)
            case 0x021UL:
                break;

            // int sem_close(sem_t handle)
            case 0x022UL:
                break;

            // int sem_wait(sem_t id)
            case 0x023UL:
                break;

            // int sem_signal(sem_t id)
            case 0x024UL:
                break;

            // int time_sleep(time_t t)
            case 0x031UL:
                break;

            // char getc()
            case 0x041UL:
                break;

            // void putc(char c)
            case 0x042UL:
                break;

            default:
                break;
        }

        RiscV::mc_sip(RiscV::SIP_SSIP);
        w_sepc(sepc + 4);
        w_sstatus(sstatus);

    } else if (scause == 0x8000000000000001UL) {


    } else if (scause == 0x8000000000000009UL) {
        console_handler();
    } else {
        // default
    }


}
