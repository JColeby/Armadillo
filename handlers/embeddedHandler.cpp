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
            // we don't use drainPipe as it will read indefinitely until it receives EoF, which won't happen since there isn't another thread to kill the writeHandle
            constexpr DWORD chunkSize = 4096;
            char buffer[chunkSize];
            std::string result = "\"";
            DWORD bytesAvailable = 0;
            DWORD bytesRead = 0;
            while (true) {
                // we peek because the named pipe would wait indefinitely for more info from the buffer.
                if (!PeekNamedPipe(readHandle, nullptr, 0, nullptr, &bytesAvailable, nullptr)) { break; }
                if (bytesAvailable == 0) { break; }
                // we also need to only read the number of bytes available to avoid waiting as well
                DWORD toRead = std::min(chunkSize, bytesAvailable);
                if (!ReadFile(readHandle, buffer, toRead, &bytesRead, nullptr) || bytesRead == 0) { break; }
                result.append(buffer, bytesRead);
            }
            result += "\"";

            // closing up all the handles
            CloseHandle(writeHandle);
            DisconnectNamedPipe(writeHandle);
            CloseHandle(readHandle);
            tokenizedInput[i] = result;
        }
    }
}