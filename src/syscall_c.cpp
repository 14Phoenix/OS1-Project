#include "../h/syscall_c.hpp"

void *mem_alloc(size_t size) {
    size_t volatile numOfBlocks = size / MEM_BLOCK_SIZE + (size % MEM_BLOCK_SIZE > 0 ? 1 : 0);
    __asm__ volatile ("mv a1, %[size]" : : [size] "r" (numOfBlocks));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x01UL));
    __asm__ volatile ("ecall");
    uint64 volatile allocated;
    __asm__ volatile ("mv %[ptr], a0" : [ptr] "=r" (allocated));
    return (void*) allocated;
}

int mem_free(void *ptr) {
    __asm__ volatile ("mv a1, %[p]" : : [p] "r" (ptr));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x02UL));
    __asm__ volatile ("ecall");
    uint64 volatile errorCode;
    __asm__ volatile ("mv %[error], a0" : [error] "=r" (errorCode));
    return (int) errorCode;
}

int thread_create(thread_t *handle, void (*start_routine)(void *), void *arg) {
    void *stack_space = nullptr;
    if (start_routine != nullptr) stack_space = mem_alloc(DEFAULT_STACK_SIZE);
    __asm__ volatile ("mv a4, %[stack]" : : [stack] "r" (stack_space));
    __asm__ volatile ("mv a3, %[arg]" : : [arg] "r" (arg));
    __asm__ volatile ("mv a2, %[routine]" : : [routine] "r" (start_routine));
    __asm__ volatile ("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x011UL));
    __asm__ volatile ("ecall");
    uint64 volatile errorCode;
    __asm__ volatile ("mv %[error], a0" : [error] "=r" (errorCode));
    return (int) errorCode;
}

int thread_exit() {
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x012UL));
    __asm__ volatile ("ecall");
    uint64 volatile errorCode;
    __asm__ volatile ("mv %[error], a0" : [error] "=r" (errorCode));
    return (int) errorCode;
}

void thread_dispatch() {
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x013UL));
    __asm__ volatile ("ecall");
}

void thread_join(thread_t handle) {
    __asm__ volatile ("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x014UL));
    __asm__ volatile ("ecall");
}

int sem_open(sem_t *handle, unsigned init) {
    __asm__ volatile ("mv a2, %[init]" : : [init] "r" (init));
    __asm__ volatile ("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x021UL));
    __asm__ volatile ("ecall");
    uint64 volatile errorCode;
    __asm__ volatile ("mv %[error], a0" : [error] "=r" (errorCode));
    return (int) errorCode;
}

int sem_close(sem_t handle) {
    __asm__ volatile ("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x022UL));
    __asm__ volatile ("ecall");
    uint64 volatile errorCode;
    __asm__ volatile ("mv %[error], a0" : [error] "=r" (errorCode));
    return (int) errorCode;
}

int sem_wait(sem_t id) {
    __asm__ volatile ("mv a1, %[id]" : : [id] "r" (id));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x023UL));
    __asm__ volatile ("ecall");
    uint64 volatile errorCode;
    __asm__ volatile ("mv %[error], a0" : [error] "=r" (errorCode));
    return (int) errorCode;
}

int sem_signal(sem_t id) {
    __asm__ volatile ("mv a1, %[id]" : : [id] "r" (id));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x024UL));
    __asm__ volatile ("ecall");
    uint64 volatile errorCode;
    __asm__ volatile ("mv %[error], a0" : [error] "=r" (errorCode));
    return (int) errorCode;
}

int time_sleep(time_t time) {
    __asm__ volatile ("mv a1, %[time]" : : [time] "r" (time));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x031UL));
    __asm__ volatile ("ecall");
    uint64 volatile errorCode;
    __asm__ volatile ("mv %[error], a0" : [error] "=r" (errorCode));
    return (int) errorCode;
}

char getc() {
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x041UL));
    __asm__ volatile ("ecall");
    char volatile character;
    __asm__ volatile ("mv %[c], a0" : [c] "=r" (character));
    return character;
}

void putc(char c) {
    __asm__ volatile ("mv a1, %[c]" : : [c] "r" (c));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (0x042UL));
    __asm__ volatile ("ecall");
}

