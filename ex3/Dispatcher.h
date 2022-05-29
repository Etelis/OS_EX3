//
// Created by kali on 5/29/22.
//

#ifndef EX3_DISPATCHER_H
#define EX3_DISPATCHER_H

#include "iostream"
#include "UnBoundedBuffer.h"
#include "BoundedBuffer.h"
#include "strings.h"

/**
 * Dispatcher.
 */
class Dispatcher {
private:
    // Three Unbounded buffer for Sports, News, Weather accordingly.
    UnBoundedBuffer* _SNWBuffers[3];
    // list of Bounded buffer for each producer.
    list<BoundedBuffer*> _producers;
public:
    Dispatcher(){
        _SNWBuffers[0] = new UnBoundedBuffer();
        _SNWBuffers[1] = new UnBoundedBuffer();
        _SNWBuffers[2] = new UnBoundedBuffer();
    }

    /**
     * get buffer by it's id.
     * @param id - id.
     * @return
     */
    UnBoundedBuffer* getBufferByID(int id);
    /**
     * dispatch - thread function to performe dispatch task.
     * @param args - the dispatcher itself converted into void*
     * @return NA
     */
    static void* dispatch(void* args);
    // Add new producer.
    void addProducer(BoundedBuffer* producerBuffer);
    // sort given product received from producer by it's type.
    void sorter(char *product, int& doneCounter);
    // signal to all buffers done reading.
    void signalDone();
};


#endif //EX3_DISPATCHER_H
