#ifndef PROJECT_BASE_H_PCB_H
#define PROJECT_BASE_H_PCB_H


#include "../lib/hw.h"
#include "./MemoryAllocator.hpp"
#include "./RiscV.hpp"
#include "./Scheduler.hpp"
#include "./PCB_Queue.hpp"


class PCB {
public:
    ~PCB();

    using Body = void (*)(void*);

    static PCB* createPCB(Body body, void *arg, void* stack);

    static PCB* getRunning();

    static void setRunning(PCB* newRunning);

    static time_t getTimeSliceCounter();

    static void setTimeSliceCounter(time_t value);

    static void incTimeSliceCounter();

    static void dispatch();

    static void yield();

    static void join(PCB *pcb);

    static void wait(time_t time);

    static void updateWait();

    enum PCBState {
        RUNNING = 0,
        READY = 1,
        BLOCKED = 2,
        WAITING = 3,
        SLEEPING = 4,
        FINISHED = 5
    };

    PCBState getState() const;

    void setState(PCBState newState);

    PCB* getNext() const;

    void setNext(PCB* newNext);

    int getSemWaitRet() const;

    void setSemWaitRet(int ret);

    time_t getTimeToSleep() const;

    void setTimeToSleep(time_t time);

    time_t getTimeSlice() const;

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

    time_t timeSlice = DEFAULT_TIME_SLICE;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static PCB *running;
    static PCB *waitHead;
    static time_t timeSliceCounter;

    PCBState state;
    PCB *next = nullptr;

    int semWaitRet = 0;

    PCB_Queue joinQueue;

    time_t timeToSleep = 0;

};


#endif // PROJECT_BASE_H_PCB_H