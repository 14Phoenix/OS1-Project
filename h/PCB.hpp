#ifndef PROJECT_BASE_H_PCB_H
#define PROJECT_BASE_H_PCB_H


#include "../lib/hw.h"
#include "./MemoryAllocator.hpp"
#include "./Scheduler.hpp"


class PCB {
public:
    using Body = void (*)(void*);

    static PCB* createPCB(Body body, void *arg, void* stack);

    bool isFinished() const;

    void setFinished(bool finished);

    static void yield();

    static PCB *running;

private:
    PCB(Body body, void *arg, void* stack) :
    body(body),
    arg(arg),
    stack(body != nullptr ? stack : nullptr),
    context({
        (uint64) &threadWrapper,
        stack != nullptr ? (uint64) &(((uint8*)stack)[DEFAULT_STACK_SIZE]) : 0
    }) {
        if (body != nullptr) Scheduler::put(this);
    }

    void* operator new(size_t size);

    void operator delete(void *ptr);

    struct Context {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    void *arg;
    void *stack;
    Context context;

    bool finished;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static uint64 timeSliceCounter;

};


#endif // PROJECT_BASE_H_PCB_H