#include <windows.h>
#include <iostream>

int main() {
    HANDLE hRead, hWrite;
    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };

    // Create a single pipe
    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
        std::cerr << "Failed to create pipe\n";
        return 1;
    }

    STARTUPINFO si = { sizeof(si) };
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hRead;   // child reads from this
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE); // child's stdout normal

    PROCESS_INFORMATION pi;

    if (!CreateProcess(
            NULL,
            (LPSTR)"child.exe", // your child process
            NULL, NULL,
            TRUE, 0, NULL, NULL,
            &si, &pi)) {
        std::cerr << "CreateProcess failed\n";
        return 1;
            }

    CloseHandle(hRead); // parent doesn't read
    const char* msg = "Hello child!\n";
    DWORD written;
    WriteFile(hWrite, msg, strlen(msg), &written, NULL);
    CloseHandle(hWrite); // signal EOF

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
