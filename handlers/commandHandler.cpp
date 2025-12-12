#include "headers/commandHandler.h"
#include "headers/embeddedHandler.h"
#include "../path.h"
#include "../commonFunctions/isStringInFile.h"




string getCommandPath(string cmd)
{
    std::unordered_set<std::string> visited; // help prevent infinite alias loops

    string alias = isStringInFile(cmd, ARDO_PATH + "/cmd/aliases.config");
    while (!alias.empty())
    {
        if (visited.count(cmd)) {
            std::cerr << "ERROR: Alias cycle detected involving '" << cmd << "'\n";
            return "";
        }
        visited.insert(cmd);

        std::size_t pos = alias.find("->");
        if (pos != std::string::npos) {
            cmd = alias.substr(pos + 2); // +2 to skip "->"
            cmd.erase(0, cmd.find_first_not_of(" \t")); // Remove leading spaces
            cmd.erase(cmd.find_last_not_of(" \t") + 1); // Remove trailing spaces
        } else {
            std::cerr << "ERROR: Alias malformed. Could not execute the desired command" << endl;
            return "";
        }
        alias = isStringInFile(cmd, ARDO_PATH + "/cmd/aliases.config");
    }
    if (!isStringInFile(cmd, ARDO_PATH + "/cmd/standardList.config").empty())
    {
        return ARDO_PATH + "/cmd/standard/" + cmd + "/" + cmd + ".exe";
    }
    if (!isStringInFile(cmd, ARDO_PATH + "/cmd/customList.config").empty())
    {
        return ARDO_PATH + "/cmd/custom/" + cmd + "/" + cmd + ".exe";
    }
    std::cerr << "ERROR: Command '" + cmd + "' does not exist or isn't listed as a valid command/alias." << endl;
    return "";
}



void commandHandler(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, bool closeWriteOnFinish)
{
    bool ownsReadHandle = (readHandle != nullptr);
    bool ownsWriteHandle = (writeHandle != nullptr);

    string commandFilePath = getCommandPath(tokenizedInput[0]);
    if (commandFilePath.empty() or !std::filesystem::exists(commandFilePath)) {
        if (!commandFilePath.empty()) {
            std::cerr << "ERROR: Command executable was not found. Expected path of executable: '" + commandFilePath + "'" << endl;
        }
        if (ownsReadHandle) { CloseHandle(readHandle); }
        if (ownsWriteHandle and closeWriteOnFinish) { CloseHandle(writeHandle); }
        return;
    }


    // check and handle embedded commands!!!!



    // setting up our command-line arguments to pass into the executable
    std::string cmdline = "\"" + commandFilePath + "\"";
    for (size_t i = 1; i < tokenizedInput.size(); i++) {
        cmdline += " " + tokenizedInput[i];
    }


    if (readHandle == nullptr) { readHandle = GetStdHandle(STD_INPUT_HANDLE); }
    if (writeHandle == nullptr) { writeHandle = GetStdHandle(STD_OUTPUT_HANDLE); }


    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = readHandle;   // child reads from this
    si.hStdOutput = writeHandle; // child's stdout normal
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    PROCESS_INFORMATION pi;


    if (!CreateProcess(NULL, cmdline.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed. Error: " << GetLastError() << "\n";
        if (ownsReadHandle) { CloseHandle(readHandle); }
        if (ownsWriteHandle and closeWriteOnFinish) { CloseHandle(writeHandle); }
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (ownsReadHandle) { CloseHandle(readHandle); }
    if (ownsWriteHandle and closeWriteOnFinish) { CloseHandle(writeHandle); }

}
