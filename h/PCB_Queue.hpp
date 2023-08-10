#ifndef PROJECT_BASE_H_PCB_QUEUE_H
#define PROJECT_BASE_H_PCB_QUEUE_H


class PCB;

class PCB_Queue {
public:
    PCB* get();

    void put(PCB *pcb);

private:
    PCB *head = nullptr, *tail = nullptr;

};


#endif // PROJECT_BASE_H_PCB_QUEUE_H