#include "../h/CharBuffer.hpp"
#include "../h/syscall_c.hpp"

CharInputBuffer *CharInputBuffer::getInstance() {
    static CharInputBuffer instance;
    return &instance;
}

char CharInputBuffer::getc() {
    itemAvailable->wait();
    char character = buffer[head];
    head = (head + 1) % BUFFER_SIZE;
    count--;
    return character;
}

void CharInputBuffer::putc(char c) {
    buffer[tail] = c;
    tail = (tail + 1) % BUFFER_SIZE;
    count++;
    itemAvailable->signal();
}

bool CharInputBuffer::isFull() const {
    return count >= BUFFER_SIZE;
}

void CharInputBuffer::freeSem() {
    K_Semaphore::deleteK_Semaphore(itemAvailable);
}

CharOutputBuffer *CharOutputBuffer::getInstance() {
    static CharOutputBuffer instance;
    return &instance;
}

char CharOutputBuffer::getc() {
    sem_wait(itemAvailable);
    char character = buffer[head];
    head = (head + 1) % BUFFER_SIZE;
    count--;
    sem_signal(spaceAvailable);
    return character;
}

void CharOutputBuffer::putc(char c) {
    spaceAvailable->wait();
    buffer[tail] = c;
    tail = (tail + 1) % BUFFER_SIZE;
    count++;
    itemAvailable->signal();
}

bool CharOutputBuffer::isEmpty() const {
    return count == 0;
}

void CharOutputBuffer::freeSem() {
    K_Semaphore::deleteK_Semaphore(itemAvailable);
    K_Semaphore::deleteK_Semaphore(spaceAvailable);
}
