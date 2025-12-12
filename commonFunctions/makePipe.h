#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <windows.h>

inline bool makePipe(HANDLE& writeHandle, HANDLE& readHandle) {
    SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
    writeHandle = nullptr; // set these to nullptr so nothing straight up breaks if windows fails to create a pipe
    readHandle = nullptr;

    if (!CreatePipe(&readHandle, &writeHandle, &saAttr, 0)) { std::cerr << "Failed to create pipe. Exiting Early" << std::endl; return false; }

    // Ensure the read handle is NOT inherited:
    SetHandleInformation(readHandle, HANDLE_FLAG_INHERIT, 0);
    return true;
}