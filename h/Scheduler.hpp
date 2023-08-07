#ifndef PROJECT_BASE_H_SCHEDULER_H
#define PROJECT_BASE_H_SCHEDULER_H


#include "../lib/hw.h"


class PCB;

class Scheduler {
public:
    static PCB* get();

    static void put(PCB *pcb);

    static Scheduler* getInstance();

private:
    PCB *readyHead = nullptr, *readyTail = nullptr;
//    struct Node {
//        Node *next;
//        PCB *pcb;
//    };
//
//    Node *head, *tail;


};


#endif // PROJECT_BASE_H_SCHEDULER_H