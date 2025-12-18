#include "headers/embeddedHandler.h"
#include "headers/inputHandler.h"
#include "../commonFunctions/handleIO.h"

using HDL::drainPipe;
using HDL::makePipe;

// handles the logic for commands that are embedded into others
void embeddedHandler(vector<string>& tokenizedInput)
{
    for (int i = 0; i < tokenizedInput.size(); i++)
    {
        if (tokenizedInput[i].size() >= 3 and tokenizedInput[i].rfind("$(", 0) == 0)
        {
            // remove syntax remnants
            string command = tokenizedInput[i].substr(2);
            if (!command.empty() && command.back() == ')') {
                command.pop_back();
            }

            // create pipe and send it back into inputHandler
            HANDLE writeHandle, readHandle;
            makePipe(writeHandle, readHandle);
            inputHandler(command, writeHandle);

            // save the command output as a string and place it back into the command
            CloseHandle(writeHandle);
            string result = "\"" + drainPipe(readHandle) + "\"";
            CloseHandle(readHandle);
            tokenizedInput[i] = result;
        }
    }
}