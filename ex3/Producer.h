//
// Created by kali on 5/29/22.
//

#ifndef EX3_PRODUCER_H
#define EX3_PRODUCER_H

#include "BoundedBuffer.h"

/**
 * Producer.
 */
class Producer {
private:
    // count for each category: Sports, News, Wether.
    int _categoryCount[3] = {0, 0, 0}, _productsToProduce, _id;
    // bounded buffer assigned for each producer.
    BoundedBuffer *_productBuffer;
    /**
     * produceString - produce string product.
     * @return product.
     */
    char* produceString();

public:
    /**
     * Construcotr.
     * @param id  - procuder's id.
     * @param productsToProduce - product count to produce.
     * @param queueSize - queue size.
     */
    Producer(int id, int productsToProduce, int queueSize): _id(id),
    _productsToProduce(productsToProduce), _productBuffer(new BoundedBuffer(queueSize))
    {};

    /**
     * produce - produce all products.
     * @param args - producer.
     * @return NA
     */
    static void* produce(void* args);
    BoundedBuffer* getProducerBuffer();
};


#endif //EX3_PRODUCER_H
