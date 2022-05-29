//
// Created by kali on 5/29/22.
//

#include "Dispatcher.h"
#define SPORTS "SPORTS"
#define NEWS "NEWS"
#define WEATHER "WEATHER"
#define DONE "DONE"

#define SPORT_INDEX 0
#define NEWS_INDEX 1
#define WEATHER_INDEX 2

using namespace std;
UnBoundedBuffer *Dispatcher::getBufferByID(int id) {
    return _SNWBuffers[id];
}

void* Dispatcher::dispatch(void* args) {
    // Conversion from void*
    auto dispatcher = (Dispatcher*) args;
    int doneCounter = (int) dispatcher->_producers.size();

    // untill received all done signals from producers keep reading.
    while (doneCounter){
        // Implementation of round robin.
        for (auto buffer : dispatcher->_producers){
            dispatcher->sorter(buffer->remove(), doneCounter);
        }
    }
    // signal done to all buffers.
    dispatcher->signalDone();
    return (void*) nullptr;
}

void Dispatcher::addProducer(BoundedBuffer *producerBuffer) {
    _producers.push_back(producerBuffer);
}

void Dispatcher::sorter(char *product, int& doneCounter) {
    if (product == nullptr)
        return;
    //cout << "Received " << product << " In dispatcher" << endl;
    string sProduct = product;

    if (sProduct.find(DONE) != -1)
        doneCounter--;

    else if (sProduct.find(SPORTS) != -1)
        _SNWBuffers[SPORT_INDEX]->insert(product);

    else if (sProduct.find(NEWS) != -1)
        _SNWBuffers[NEWS_INDEX]->insert(product);

    else if (sProduct.find(WEATHER) != -1)
        _SNWBuffers[WEATHER_INDEX]->insert(product);

}

void Dispatcher::signalDone() {
    _SNWBuffers[SPORT_INDEX]->insert(DONE);
    _SNWBuffers[NEWS_INDEX]->insert(DONE);
    _SNWBuffers[WEATHER_INDEX]->insert(DONE);
}
