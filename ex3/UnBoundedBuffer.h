//
// Created by kali on 5/29/22.
//

#ifndef EX3_UNBOUNDEDBUFFER_H
#define EX3_UNBOUNDEDBUFFER_H
#include <iostream>
#include "list"
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <semaphore>

using namespace std;
class UnBoundedBuffer {
private:
    // mutex for reading and writing to buffer.
    pthread_mutex_t _mutex;
    // one side counting semaphore as it's an unbounded buffer.
    int _full = 0;
    // buffer.
    list<char*> _buffer;
    // regular wait blocking function.
    static void wait(int &semaphore);
    // regular signal function.
    static void signal(int &semaphore);

public:
    // thread-safe insert function.
    void insert (char *s);
    // thread-safe remove function.
    char* remove();
};


#endif //EX3_UNBOUNDEDBUFFER_H
