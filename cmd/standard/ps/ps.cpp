#pragma comment(lib, "Psapi.lib")
#include <filesystem>
#include <iostream>
#include <string>
#include <tchar.h>
#include <unordered_set>
#include <vector>
#include <windows.h>
#include <psapi.h>

#include "../../../TerminalFormatting.h"

using namespace VT;
using std::to_string;


using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;


struct Options {
  bool showFullFilepath = false;
  bool hideDuplicates = false;
};



bool validateSyntaxAndSetFlags(std::vector<std::string> &tokenizedInput, Options& opt) {
  for (size_t i = 1; i < tokenizedInput.size(); i++) { // looping through each parameter
    const std::string& param = tokenizedInput[i];

    // setting flags
    if (param[0] == '-') {
      for (int j = 1; j < param.size(); j++) {
        switch (param[j]) {
        case 'a': opt.showFullFilepath = true; break;
        case 'b': opt.hideDuplicates = true; break;
        default: cerr << "SYNTAX ERROR: Unexpected flag: -" << param[j] << endl; return false;
        }
      }
    }
    else {
      cerr << "SYNTAX ERROR: Unexpected parameter: " << param << endl; return false;
    }
  }
  return true;
}


string utf8_encode(const std::wstring& wstr) {
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
  string str(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, nullptr, nullptr);
  return str;
}



int main(int argc, char* argv[]) {
  // standard setup
  Options opt;
  std::vector<std::string> tokenizedInput(argv, argv + argc); // convert it to a proper array
  if (!validateSyntaxAndSetFlags(tokenizedInput, opt)) { return -1; }

  std::unordered_set<string> runningProcesses;

  const char* white = "";
  const char* reset = "";

  if (verify()) {
    white = WHITE;
    reset = RESET_TEXT;
  }

  // getting the process ids for all active processes
  DWORD processes[1024], bytesReturned;
  if (!EnumProcesses(processes, sizeof(processes), &bytesReturned)) {
    cerr << "Failed to get processes. System error message: " << to_string(GetLastError()) << endl;
    return -1;
  }

  std::stringstream outputBuffer;
  outputBuffer << white << "PID:            Executable:\n" << reset;

  // looping through each process id that was returned
  unsigned int count = bytesReturned / sizeof(DWORD);
  for (unsigned int i = 0; i < count; ++i) {

    // getting the current process
    DWORD pid = processes[i];
    if (pid == 0) { continue; } // skip system idle process
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!process) { continue; } // skip if we can't get the process

    // getting the filepath
    wchar_t exeName[MAX_PATH] = L"[Unknown]";     // wchar is used so we can handle unicode characters correctly
    if (!GetModuleFileNameExW(process, nullptr, exeName, MAX_PATH)) {
      wcscpy_s(exeName, L"[Access Denied]");
    }
    string processName = utf8_encode(exeName); // encode it back to normal char characters

    // flag adjustments
    if (!opt.showFullFilepath) { processName = std::filesystem::path(processName).filename().string(); } // gets only the filename
    if (opt.hideDuplicates) {
      if (runningProcesses.find(processName) == runningProcesses.end()) { runningProcesses.insert(processName); }
      else { CloseHandle(process); continue; }
    }

    // save the process to the buffer
    string itemBuffer = "  " + to_string(pid);
    while (itemBuffer.size() < 16) { itemBuffer += " "; }
    outputBuffer << itemBuffer << "  " << processName << "\n";

    CloseHandle(process);
  }

  cout << outputBuffer.str();
  return 0;
}


