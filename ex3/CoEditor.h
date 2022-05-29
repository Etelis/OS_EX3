//
// Created by kali on 5/29/22.
//

#ifndef EX3_COEDITOR_H
#define EX3_COEDITOR_H

#include "iostream"
#include "cstring"
#include "UnBoundedBuffer.h"
#include "BoundedBuffer.h"
#include "chrono"
#include "thread"
#define DONE "DONE"

/**
 * coEditorParmas - Params for edit function, formated for void* conversion.
 */
struct coEditorParmas{
    UnBoundedBuffer* _dispatcherBuffer;
    BoundedBuffer* _SMBuffer;
    coEditorParmas(UnBoundedBuffer* dispatcherBuffer, BoundedBuffer* SMBuffer): _dispatcherBuffer(dispatcherBuffer), _SMBuffer(SMBuffer){}
};

/**
 * CO-Editor class.
 */
class CoEditor {
public:
    /**
     * Edit fucntion - will be ran by a thread.
     * @param args - args are the params declared before.
     * @return NA
     */
    static void* edit(void* args){
        // each editor has an inbound UnBoundedBuffer to receive data from, and BoundedBuffer to export data for the Screen Manager.
        auto params = (coEditorParmas*)args;
        UnBoundedBuffer* dispatcherBuffer = params->_dispatcherBuffer;
        BoundedBuffer *SMBuffer = params->_SMBuffer;

        char *productReceived;
        while (true){
            // remove an item from Dispacher's buffer.
            productReceived = dispatcherBuffer->remove();
            // insert same item to Screen manager's buffer, after blocking.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            SMBuffer->insert(productReceived);
            if (!strcmp(productReceived, DONE))
                break;
        }

        return (void*) nullptr;
    }
};

#endif //EX3_COEDITOR_H
