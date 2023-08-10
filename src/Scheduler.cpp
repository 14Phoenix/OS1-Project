#include "../h/Scheduler.hpp"
#include "../h/PCB.hpp"

PCB *Scheduler::get() {
    /*
    PCB *pcb = getInstance()->readyHead;
    if (getInstance()->readyHead != nullptr) {
//        pcb = getInstance()->readyHead;
        getInstance()->readyHead = getInstance()->readyHead->getNext();
//        getInstance()->readyHead->setPrevious(nullptr);
    }
    if (getInstance()->readyHead == nullptr) {
//        pcb = emptyThread;
        getInstance()->readyTail = nullptr;
    }
    pcb->setNext(nullptr);
    */
    PCB *pcb = getInstance()->queue.get();
    return pcb;
}

void Scheduler::put(PCB *pcb) {
    pcb->setState(PCB::PCBState::READY);
    /*
    if (getInstance()->readyHead == nullptr) {
        getInstance()->readyHead = pcb;
        pcb->setPrevious(nullptr);
    } else {
        getInstance()->readyTail->setNext(pcb);
        pcb->setPrevious(getInstance()->readyTail);
    }
    getInstance()->readyTail = pcb;
    pcb->setNext(nullptr);
    */
    getInstance()->queue.put(pcb);
}

Scheduler *Scheduler::getInstance() {
    static Scheduler instance;
    return &instance;
}
