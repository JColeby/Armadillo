#include "headers/embeddedHandler.h"
#include "headers/inputHandler.h"
#include "../commonFunctions/makePipe.h"

void embeddedHandler(vector<string>& tokenizedInput)
{
    for (int i = 0; i < tokenizedInput.size(); i++)
    {
        if (tokenizedInput[i].size() >= 3 and tokenizedInput[i].rfind("$(", 0) == 0)
        {
            string command = tokenizedInput[i].substr(2, tokenizedInput[i].size() - 3);
            command.pop_back();

            // create pipe and send it back into inputHandler
            HANDLE writeHandle, readHandle;
            makePipe(writeHandle, readHandle);
            inputHandler(command, writeHandle);
            CloseHandle(writeHandle);

            // Create a buffer to read from the stdout of the embedded command
            const DWORD bufferSize = 4096;
            char buffer[bufferSize];
            DWORD bytesRead = 0;
            string result = "\"";

            // Read until there’s nothing left
            while (true) {
                BOOL success = ReadFile(readHandle, buffer, bufferSize, &bytesRead, nullptr);
                if (!success || bytesRead == 0) break; // pipe closed or error
                result.append(buffer, bytesRead);
            }

            result.append("\"");

            tokenizedInput[i] = result;
        }
    }
}