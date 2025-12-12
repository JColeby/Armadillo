#include "headers/pipeHandler.h"
#include "headers/commandHandler.h"
#include "../commonFunctions/separateTokensByToken.h"
#include "../commonFunctions/makePipe.h"


void pipeHandler(const vector<string>& tokenizedInput, HANDLE& finalWriteHandle) {
    vector<vector<string>> individualCommands = separateTokensByToken(tokenizedInput, "|");
    if (individualCommands.size() == 1) { commandHandler(individualCommands[0], nullptr, nullptr, false); return; }

    std::vector<std::thread> threads;

    HANDLE previousReadHandle = nullptr; // becomes the handle that the previous command will read from
    for (int i = 0; i < individualCommands.size() - 1; i++) {

        HANDLE writeHandle, readHandle;
        makePipe(writeHandle, readHandle);

        // create a new thread that calls commandHandler, which will create the new processes and set their stdin and stdout handles
        threads.emplace_back(commandHandler, individualCommands[i], previousReadHandle, writeHandle, true);

        previousReadHandle = readHandle; // next command will now be able to read from this handle
    }

    // the very last thread should write to stdout
    threads.emplace_back(commandHandler, individualCommands[individualCommands.size()-1], previousReadHandle, finalWriteHandle, false);

    for (auto& t : threads) {
        t.join();
    }

}