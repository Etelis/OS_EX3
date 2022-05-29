//
// Created by kali on 5/29/22.
//

#include "UnBoundedBuffer.h"

void UnBoundedBuffer::insert(char *s) {
    pthread_mutex_lock(&_mutex);
    _buffer.push_back(s);
    pthread_mutex_unlock(&_mutex);
    signal(_full);
}

char *UnBoundedBuffer::remove() {
    char *item;
    wait(_full);
    pthread_mutex_lock(&_mutex);
    item = _buffer.front();
    _buffer.pop_front();
    pthread_mutex_unlock(&_mutex);
    return item;
}

void UnBoundedBuffer::wait(int &semaphore) {
    while (!semaphore);
    semaphore--;
}

void UnBoundedBuffer::signal(int &semaphore) {
    semaphore++;
}
