#pragma once
#include <windows.h>
#include <string>
#include <iostream>

/**
 * Reads all remaining data from a pipe handle and returns it as a std::string.
 * This function blocks until the writer closes the pipe (EOF).
 */
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
