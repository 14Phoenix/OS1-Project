#ifndef PROJECT_BASE_H_PCB_H
#define PROJECT_BASE_H_PCB_H


#include "../lib/hw.h"
#include "./MemoryAllocator.hpp"
#include "./RiscV.hpp"
#include "./Scheduler.hpp"


class PCB {
public:
    ~PCB();

    using Body = void (*)(void*);

    static PCB* createPCB(Body body, void *arg, void* stack);

//    bool isFinished() const;

//    void setFinished(bool finished);

    static PCB* getRunning();

    static void setRunning(PCB* newRunning);

    static uint64 getTimeSliceCounter();

    static void setTimeSliceCounter(uint64 value);

    static void dispatch();

    static void yield();

    enum PCBState {
        RUNNING = 0,
        READY = 1,
        BLOCKED = 2,
        WAITING = 3,
        FINISHED = 4
    };

    PCBState getState() const;

    void setState(PCBState newState);

    PCB* getNext() const;

    void setNext(PCB* newNext);

    int getSemWaitRet() const;

    void setSemWaitRet(int ret);

private:
    PCB(Body body, void *arg, void* stack) :
    body(body),
    arg(arg),
    stack(body != nullptr ? stack : nullptr),
    context({
        (uint64) &threadWrapper,
        stack != nullptr ? (uint64) &(((uint8*)stack)[DEFAULT_STACK_SIZE]) : 0
    }),
    state(PCBState::READY) {}

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

//    bool finished;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static PCB *running;
    static uint64 timeSliceCounter;

    // Access from RiscV
//    friend class RiscV;

    // Scheduler linked list
//    friend class Scheduler;

    PCBState state;
    PCB *next = nullptr;

    int semWaitRet = 0;

};


#endif // PROJECT_BASE_H_PCB_H