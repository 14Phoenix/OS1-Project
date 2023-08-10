#include "../h/PCB_Queue.hpp"
#include "../h/PCB.hpp"

PCB *PCB_Queue::get() {
    PCB *pcb = head;
    if (head != nullptr) {
        head = head->getNext();
    }
    if (head == nullptr) {
        tail = nullptr;
    }
    return pcb;
}

void PCB_Queue::put(PCB *pcb) {
    if (head == nullptr) {
        head = pcb;
    } else {
        tail->setNext(pcb);
    }
    tail = pcb;
    tail->setNext(nullptr);
}