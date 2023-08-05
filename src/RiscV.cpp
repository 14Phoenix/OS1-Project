#include "../h/RiscV.hpp"

#include "../lib/console.h"

void RiscV::popSppSpie() {
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void RiscV::handleSupervisorTrap() {
    uint64 scause = r_scause();

    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {

        uint64 volatile syscallCode;
        __asm__ volatile ("mv %[code], a0" : [code] "=r" (syscallCode));

        switch (syscallCode) {
            // void* mem_alloc(size_t size)
            case 0x01UL: {
                uint64 volatile numOfBlocks;
                __asm__ volatile ("mv %[size], a1" : [size] "=r" (numOfBlocks));
                void *allocated = MemoryAllocator::getInstance()->memAlloc(numOfBlocks * MEM_BLOCK_SIZE);
                __asm__ volatile ("mv a0, %[ptr]" : : [ptr] "r" ((uint64) allocated));
                break;
            }

            // int mem_free(void *ptr)
            case 0x02UL: {
                uint64 volatile ptr;
                __asm__ volatile ("mv %[p], a1" : [p] "=r" (ptr));
                int errorCode = MemoryAllocator::getInstance()->memFree((void *) ptr);
                __asm__ volatile ("mv a0, %[error]" : : [error] "r" ((uint64) errorCode));
                break;
            }

            // int thread_create(thread_t *handle, void(*start_routine)(void*), void* arg, void* stack_space)
            case 0x011UL: {
                uint64 volatile handle;
                __asm__ volatile ("mv %[handle], a1" : [handle] "=r" (handle));
                uint64 volatile startRoutine;
                __asm__ volatile ("mv %[routine], a2" : [routine] "=r" (startRoutine));
                uint64 volatile arguments;
                __asm__ volatile ("mv %[arg], a3" : [arg] "=r" (arguments));
                uint64 volatile stackSpace;
                __asm__ volatile ("mv %[stack], a4" : [stack] "=r" (stackSpace));

                PCB *pcb = PCB::createPCB((PCB::Body) startRoutine, (void*) arguments, (void*) stackSpace);
                int errorCode = 0;
                if (pcb == nullptr) errorCode = -1;

                __asm__ volatile ("mv a1, %[pcb]" : : [pcb] "r" ((uint64) pcb));
                __asm__ volatile ("mv a0, %[code]" : : [code] "r" ((uint64) errorCode));
                break;
            }

            // int thread_exit()
            case 0x012UL:
                break;

            // void thread_dispatch()
            case 0x013UL:
                break;

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

    } else if (scause == 0x8000000000000001UL) {


    } else if (scause == 0x8000000000000009UL) {
        console_handler();
    } else {
        // default
    }


}
