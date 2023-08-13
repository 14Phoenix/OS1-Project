#ifndef PROJECT_BASE_H_CHARBUFFER_H
#define PROJECT_BASE_H_CHARBUFFER_H


#include "../lib/hw.h"
#include "./K_Semaphore.hpp"


class CharInputBuffer {
public:
    static CharInputBuffer* getInstance();

    char getc();

    void putc(char c);

    bool isFull() const;

private:
    CharInputBuffer() :
    head(0), tail(0), count(0) {
        itemAvailable = K_Semaphore::createK_Semaphore(0);
    }

    /*~CharInputBuffer() {
        K_Semaphore::deleteK_Semaphore(itemAvailable);
    }*/

    static const size_t BUFFER_SIZE = 512;

    char buffer[BUFFER_SIZE];
    size_t head, tail, count;
    K_Semaphore *itemAvailable;

};

class CharOutputBuffer {
public:
    static CharOutputBuffer* getInstance();

    char getc();

    void putc(char c);

private:
    CharOutputBuffer() :
    head(0), tail(0), count(0) {
        itemAvailable = K_Semaphore::createK_Semaphore(0);
        spaceAvailable = K_Semaphore::createK_Semaphore(BUFFER_SIZE);
    }

    /*~CharOutputBuffer() {
        K_Semaphore::deleteK_Semaphore(itemAvailable);
        K_Semaphore::deleteK_Semaphore(spaceAvailable);
    }*/

    static const size_t BUFFER_SIZE = 512;

    char buffer[BUFFER_SIZE];
    size_t head, tail, count;
    K_Semaphore *itemAvailable, *spaceAvailable;

};


#endif // PROJECT_BASE_H_CHARBUFFER_H