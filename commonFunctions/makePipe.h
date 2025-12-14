#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <windows.h>
#include <sstream>
#include <atomic>

// global atomic counter for unique pipe names
inline std::atomic<int> gPipeCounter{0};

// allows you to more easily create a pipe using named pipes
inline bool makePipe(HANDLE& writeHandle, HANDLE& readHandle) {
    SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
    writeHandle = nullptr; // setting these to null in case something goes wrong. Will make it so ARDO doesn't crash
    readHandle = nullptr;

    // generate a unique pipe name
    int counter = gPipeCounter.fetch_add(1, std::memory_order_relaxed);
    std::stringstream ss;
    ss << "\\\\.\\pipe\\ArdoPipe_" << GetCurrentProcessId() << "_" << counter;
    std::string pipeName = ss.str();

    // create the server side (write handle for parent writing)
    HANDLE hWritePipe = CreateNamedPipeA(
        pipeName.c_str(),
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        1,
        4096, 4096,
        0,
        &saAttr
    );

    if (hWritePipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create named pipe (server side). Error: " << GetLastError() << std::endl;
        return false;
    }

    // create the client side (read handle for child)
    HANDLE hReadPipe = CreateFileA(
        pipeName.c_str(),
        GENERIC_READ,
        0,
        &saAttr,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hReadPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open named pipe (client side). Error: " << GetLastError() << std::endl;
        CloseHandle(hWritePipe);
        return false;
    }

    // connect the server side
    BOOL connected = ConnectNamedPipe(hWritePipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    if (!connected) {
        std::cerr << "ConnectNamedPipe failed: " << GetLastError() << std::endl;
        CloseHandle(hWritePipe);
        CloseHandle(hReadPipe);
        return false;
    }

    writeHandle = hWritePipe;
    readHandle = hReadPipe;

    return true;
}
