#pragma once
#include <windows.h>
#include <iostream>
#include <sstream>
#include <atomic>


// File contains common functions that can be used to create pipes and files that commands will read/write to


namespace HDL {

// used to determine if we should display colored text
inline bool isHandleTerminal(HANDLE h) {
  if (h == nullptr || h == INVALID_HANDLE_VALUE) return false;
  DWORD fileType = GetFileType(h);
  switch (fileType) {
  case FILE_TYPE_CHAR:
    // Could be a console
    DWORD mode;
    if (GetConsoleMode(h, &mode)) {
      return true;  // It's a console/terminal
    }
    return false; // Some other character device
  case FILE_TYPE_PIPE:
    return false; // Pipe
  case FILE_TYPE_DISK:
    return false; // Regular file
  default:
    return false; // Unknown or remote device
  }
}



// creates a file handle that the command will write to
// append is a boolean that dictates whether to append onto the given file or to
// erase it
inline bool openWriteFileHandle(HANDLE& newHandle, const string& fileName, bool append) {
  SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };

  DWORD creation = append ? OPEN_ALWAYS : CREATE_ALWAYS;
  newHandle = CreateFileA(
      fileName.c_str(),
      GENERIC_WRITE,
      FILE_SHARE_READ,
      &saAttr,
      creation,
      FILE_ATTRIBUTE_NORMAL,
      NULL
  );

  if (append) {
    SetFilePointer(newHandle, 0, NULL, FILE_END);
  }

  if (newHandle == INVALID_HANDLE_VALUE) {
    newHandle = nullptr;
    return false;
  }
  return true;
}



// creates a handle to a file that the command will read from
inline bool openInputFileHandle(HANDLE& newHandle, const string& fileName) {
  SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };

  newHandle = CreateFileA(
      fileName.c_str(),
      GENERIC_READ,
      FILE_SHARE_READ,
      &saAttr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL
  );

  if (newHandle == INVALID_HANDLE_VALUE) {
    newHandle = nullptr;
    return false;
  }

  return true;
}



// used to read a line from a file/handle
inline bool readFromHandle(HANDLE readHandle, std::string& data) {
  data.clear();
  char ch;
  DWORD bytesRead;
  while (true) {
    BOOL ok = ReadFile(readHandle, &ch, 1, &bytesRead, NULL); // read in a character
    if (!ok || bytesRead == 0) { return !data.empty(); } // if EOF and no bytes were previously read, we return false
    if (ch == '\r') { continue; }
    if (ch == '\n') { return true; }
    data.push_back(ch);
  }
}



// writes to a handle. Used in builtin commands to write to the stdin and stderr handles passed in
inline bool writeToHandle(HANDLE writeHandle, const std::string& data) {
  DWORD bytesWritten;
  return WriteFile(
      writeHandle,              // pipe write HANDLE
      data.data(),              // buffer
      data.size(),              // number of bytes
      &bytesWritten,            // bytes actually written
      nullptr                   // not using overlapped I/O
  );
}


// writes to a stderr handle. Really just sets the color to red if the handle is a console.
inline bool writeToErrHandle(HANDLE writeHandle, const std::string& data) {
    CONSOLE_SCREEN_BUFFER_INFO info{};
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode = 0;
    GetConsoleMode(hOut, &consoleMode);
    if (consoleMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) {
        GetConsoleScreenBufferInfo(writeHandle, &info);
        SetConsoleTextAttribute(writeHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
    }
    bool result = writeToHandle(writeHandle, data);
    if (consoleMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) {
        SetConsoleTextAttribute(writeHandle, info.wAttributes);
    }
    return result;
}



// Reads all remaining data from a pipe handle and returns it as a std::string.
// This function blocks until the writer closes the pipe (EOF).
inline std::string drainPipe(HANDLE readHandle) {
  if (readHandle == nullptr || readHandle == INVALID_HANDLE_VALUE) {
    std::cerr << "Invalid read handle passed to drainPipe." << std::endl;
    return "";
  }

  const DWORD bufferSize = 4096;
  char buffer[bufferSize];
  DWORD bytesRead = 0;
  std::string result;

  while (true) {
    BOOL success = ReadFile(readHandle, buffer, bufferSize, &bytesRead, nullptr);
    if (!success) {
      DWORD err = GetLastError();
      if (err == ERROR_BROKEN_PIPE or err == ERROR_PIPE_NOT_CONNECTED) { break; } // Pipe closed by the writer — normal EOF
      std::cerr << "Error reading from pipe. Error code: " << err << std::endl;
      break;
    }

    if (bytesRead == 0) { break; } // reached the end of the file

    result.append(buffer, bytesRead);
  }

  return result;
}



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

}