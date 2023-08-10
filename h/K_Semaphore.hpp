#ifndef PROJECT_BASE_H_K_SEMAPHORE_H
#define PROJECT_BASE_H_K_SEMAPHORE_H


#include "../lib/hw.h"
#include "./PCB_Queue.hpp"


class PCB;

class K_Semaphore {
public:
    static K_Semaphore* createK_Semaphore(int value);

    static int deleteK_Semaphore(K_Semaphore *semaphore);

    void wait();

    void signal();

private:
    K_Semaphore(int value = 1) : value(value) {}
    ~K_Semaphore();

    int value;
    PCB_Queue queue;

    void block();

    void unblock();

    void* operator new(size_t size);

    void operator delete(void *ptr);

};


#endif // PROJECT_BASE_H_K_SEMAPHORE_H