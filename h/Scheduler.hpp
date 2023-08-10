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
    PCB_Queue queue;
//    PCB *readyHead = nullptr, *readyTail = nullptr;

};


#endif // PROJECT_BASE_H_SCHEDULER_H