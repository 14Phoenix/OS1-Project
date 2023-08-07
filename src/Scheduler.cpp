#include "../h/Scheduler.hpp"

#include "../h/PCB.hpp"

PCB *Scheduler::get() {
    PCB *pcb = getInstance()->readyHead;
    if (getInstance()->readyHead != nullptr) {
        getInstance()->readyHead = getInstance()->readyHead->next;
    }
    if (getInstance()->readyHead == nullptr) {
        getInstance()->readyTail = nullptr;
    }
    return pcb;
}

void Scheduler::put(PCB *pcb) {
    if (getInstance()->readyHead == nullptr)
        getInstance()->readyHead = pcb;
    else
        getInstance()->readyTail->next = pcb;
    getInstance()->readyTail = pcb;
    pcb->next = nullptr;
}

Scheduler *Scheduler::getInstance() {
    static Scheduler instance;
    return &instance;
}
