//
// Created by kali on 5/29/22.
//

#ifndef EX3_BOUNDEDBUFFER_H
#define EX3_BOUNDEDBUFFER_H
#include <iostream>
#include "list"
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <semaphore>

using namespace std;

/**
 * Bounded Buffer as demonstrated at the lectures.
 */
class BoundedBuffer {
private:
    // counting semaphore.
    int _size, _full, _empty;
    // buffer.
    list<char *> _buffer;
    // mutex.
    pthread_mutex_t _mutex;

    /**
     * wait function - non blocking.
     * @param semaphore the semaphore.
     * @return true - if semaphore >= 0 o.w false.
     */
    static bool wait(int &semaphore);

    // regular signal.
    static void signal(int &semaphore);

public:
    BoundedBuffer(int size): _size(size), _full(0), _empty(size) {}
    // thread-safe insert function.
    void insert(char *s);
    // thread-safe remove function.
    char* remove();
};


#endif //EX3_BOUNDEDBUFFER_H
