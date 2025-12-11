#include <windows.h>
#include <string>
#include <vector>
#include <iostream>

std::string execWindows(const std::string &exePath, const std::vector<std::string> &args)
{
    // Build command line: "exe arg1 arg2 ..."
    std::string cmd = "\"" + exePath + "\"";
    for (const auto &a : args)
        cmd += " \"" + a + "\"";

    // --- Create pipes for stdout redirection ---
    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES saAttr{};
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;       // child inherits handles
    saAttr.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0))
        throw std::runtime_error("CreatePipe failed");

    // Ensure the read handle is NOT inherited:
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    // --- Setup STARTUPINFO to redirect stdout ---
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    si.hStdOutput = hWritePipe;
    si.hStdError  = hWritePipe;   // redirect stderr too (optional)
    si.dwFlags |= STARTF_USESTDHANDLES;

    // --- Create the child process ---
    if (!CreateProcessA(
            NULL,
            cmd.data(),    // command line (writeable buffer!)
            NULL,
            NULL,
            TRUE,          // inherit handles
            0,
            NULL,
            NULL,
            &si,
            &pi))
    {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        throw std::runtime_error("CreateProcess failed");
    }

    // The child process now uses hWritePipe; close our copy:
    CloseHandle(hWritePipe);

    // --- Read child's output ---
    std::string output;
    char buffer[256];
    DWORD bytesRead;

    while (true)
    {
        BOOL success = ReadFile(hReadPipe, buffer, sizeof(buffer), &bytesRead, NULL);
        if (!success || bytesRead == 0)
            break;
        output.append(buffer, bytesRead);
    }

    // Cleanup
    CloseHandle(hReadPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return output;
}

int main()
{
    try
    {
        std::string result = execWindows("C:\\Path\\To\\otherProgram.exe",
                                         {"arg1", "arg2", "arg3"});

        std::cout << "Child output:\n" << result;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
    }
}
