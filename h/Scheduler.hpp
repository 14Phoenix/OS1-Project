#ifndef PROJECT_BASE_H_SCHEDULER_H
#define PROJECT_BASE_H_SCHEDULER_H


#include "../lib/hw.h"
#include "./PCB_Queue.hpp"


class PCB;

class Scheduler {
public:
    static PCB* get();

    static void put(PCB *pcb);

    static Scheduler* getInstance();

private:
    Scheduler();

    PCB_Queue queue;
    PCB *emptyPCB;

    static void emptyBody(void *arg);

};


#endif // PROJECT_BASE_H_SCHEDULER_H