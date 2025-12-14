#include "headers/pipeHandler.h"
#include "headers/commandHandler.h"
#include "../commonFunctions/separateTokensByToken.h"
#include "../commonFunctions/makePipe.h"



// creates a pipe and passes in the read+write handles to commandHandler, which will execute the corresponding commands
void pipeHandler(const vector<string>& tokenizedInput, HANDLE& finalWriteHandle) {
    vector<vector<string>> individualCommands = separateTokensByToken(tokenizedInput, "|");

    // if we don't have to pipe the output
    if (individualCommands.size() == 1) { commandHandler(individualCommands[0], nullptr, finalWriteHandle, false); return; }

    std::vector<std::thread> threads;

    HANDLE previousReadHandle = nullptr; // handle that the next command will read from
    for (int i = 0; i < individualCommands.size() - 1; i++) {

        HANDLE writeHandle, readHandle;
        makePipe(writeHandle, readHandle);

        // create a new thread that calls commandHandler, which will create the new processes and set their stdin and stdout handles
        threads.emplace_back(commandHandler, individualCommands[i], previousReadHandle, writeHandle, true);

        previousReadHandle = readHandle; // next command will now be able to read from this handle
    }

    // the very last thread should write to stdout (or to another handle if it's an embedded command)
    threads.emplace_back(commandHandler, individualCommands[individualCommands.size()-1], previousReadHandle, finalWriteHandle, false);

    for (auto& t : threads) {
        t.join();
    }

}