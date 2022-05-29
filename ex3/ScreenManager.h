//
// Created by kali on 5/29/22.
//

#ifndef EX3_SCREENMANAGER_H
#define EX3_SCREENMANAGER_H

#define DONE "DONE"
#include "BoundedBuffer.h"
#include "cstring"

class ScreenManager {
private:
    BoundedBuffer* _coEditorsBuffer;
public:
    ScreenManager(int coEditorBufferSize) : _coEditorsBuffer(new BoundedBuffer(coEditorBufferSize)){}
    /**
     * getEditorsBuffer - get the buffer created for all Co-Editors.
     * @return the buffer.
     */
    BoundedBuffer* getEditorsBuffer();
    /**
     * displayStrings - the function that will be ran by the separated thread to perform Screen Manager task.
     * @param args - Screen Manager object.
     * @return NA.
     */
    static void* displayStrings(void* args);

};


#endif //EX3_SCREENMANAGER_H
