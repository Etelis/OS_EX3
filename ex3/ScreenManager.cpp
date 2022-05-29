//
// Created by kali on 5/29/22.
//

#include "ScreenManager.h"

BoundedBuffer *ScreenManager::getEditorsBuffer() {
    return _coEditorsBuffer;
}

void* ScreenManager::displayStrings(void* args) {
    // Conversion from void*
    auto screenManager = (ScreenManager*)args;
    int doneCount = 3;
    char *productReceived;

    // iterate will done signal is received.
    while (doneCount){
        // if null retured (anti-blocking) just skip.
        if(!(productReceived = screenManager->_coEditorsBuffer->remove()))
            continue;

        else if (!strcmp(productReceived, DONE))
            doneCount--;

        else
            cout << productReceived << endl;
    }
    cout << DONE << endl;
}

