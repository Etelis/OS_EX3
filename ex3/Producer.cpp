//
// Created by kali on 5/29/22.
//

#include <cstring>
#include "Producer.h"
#define DONE "DONE"

void* Producer::produce(void* args) {
    // expected to receive producer.
    auto* producer = (Producer*) args;
    int i = 0;

    // produce products according to count.
    for(; i < producer->_productsToProduce; i++){
        producer->_productBuffer->insert(producer->produceString());
    }

    // when all products has been inserted insert DONE.
    producer->_productBuffer->insert(DONE);
    return ((void *) nullptr);
}

 char* Producer::produceString() {
    string choices[] = {"SPORTS", "NEWS", "WEATHER"};
    // randomly choose a category.
    int choice = rand()%3;
    // create string.
    string product = "Producer " + to_string(_id) + " " + choices[choice] + " " + to_string(++_categoryCount[choice]);
    char *c_product = new char[product.length()];
    // convert string to char.
    strcpy(c_product, product.c_str());
    return c_product;
}

BoundedBuffer *Producer::getProducerBuffer(){
        return _productBuffer;
}

