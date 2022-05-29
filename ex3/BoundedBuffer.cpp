//
// Created by kali on 5/29/22.
//

#include "BoundedBuffer.h"

void BoundedBuffer::insert(char *s) {
    // Wait on _empty.
    wait(_empty);
    // lock for inserting.
    pthread_mutex_lock(&_mutex);
    // push item.
    _buffer.push_back(s);
    pthread_mutex_unlock(&_mutex);
    // signal a new item has been added.
    signal(_full);
}

char *BoundedBuffer::remove() {
    char *item;
    // check if wait should have blocked, if it did then returen null.
    if(!wait(_full))
        return nullptr;

    // o.w remove an item and update semaphores accordingly.
    pthread_mutex_lock(&_mutex);

    item = _buffer.front();
    _buffer.pop_front();

    signal(_empty);
    pthread_mutex_unlock(&_mutex);

    return item;
}

bool BoundedBuffer::wait(int &semaphore) {
    if (!semaphore)
        return false;
    semaphore--;
    return true;
}

void BoundedBuffer::signal(int &semaphore) {
    semaphore++;
}
