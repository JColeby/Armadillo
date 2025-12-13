#pragma once
#include <windows.h>
#include <string>
#include <iostream>

// writes to a pipe handle. Used in builtin commands to write to the handle passed in
inline void writeToPipe(HANDLE writeHandle, const std::string& data)
{
    DWORD bytesWritten = 0;

    BOOL success = WriteFile(
        writeHandle,              // pipe write HANDLE
        data.data(),              // buffer
        static_cast<DWORD>(data.size()), // number of bytes
        &bytesWritten,            // bytes actually written
        nullptr                   // not using overlapped I/O
    );

    if (!success) {
        std::cerr << "WriteFile failed. Error: " << GetLastError() << "\n";
    }
}
