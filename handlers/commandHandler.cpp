#include "headers/commandHandler.h"
#include "headers/embeddedHandler.h"
#include "../path.h"
#include "../commonFunctions/isStringInFile.h"
#include "../commonFunctions/drainPipe.h"
#include "../cmd/builtin/headers/cd.h"
#include "../cmd/builtin/headers/alias.h"
#include "../cmd/builtin/headers/help.h"
#include "../cmd/builtin/headers/cmd.h"
#include "../cmd/builtin/headers/man.h"


// evaluates and executes builtin commands
bool evaluateBuiltins(vector<string>& tokenizedInput, HANDLE readHandle, HANDLE writeHandle, HANDLE errorHandle)
{
    try {
        string command = tokenizedInput[0];
        if (command == "clear") {  system("cls"); return true; }
        if (command == "cd") { cdMain(tokenizedInput, readHandle, writeHandle, errorHandle); return true; }
        if (command == "alias") { aliasMain(tokenizedInput, readHandle, writeHandle, errorHandle); return true; }
        if (command == "help") { helpMain(tokenizedInput, readHandle, writeHandle, errorHandle); return true; }
        if (command == "cmd") { cmdMain(tokenizedInput, readHandle, writeHandle, errorHandle); return true; }
        if (command == "man") { manMain(tokenizedInput, readHandle, writeHandle, errorHandle); return true; }
    } catch (const std::exception& e) {
        cerr << "ERROR: Builtin Command threw an error." << endl
             << "error message: " << e.what() << endl;
        return true;
    } catch (...) {
        cerr << "ERROR: Unknown error occurred when running a command" << endl;
        return true;
    }
    return false;
}



// finds the path to the command executable.
// Will first check to see if the command is an alias, to which it will update to the corresponding command
string getCommandPath(string cmd)
{
    // checks if the command is inside the standard list. Builds the path if the command is found
    if (!isStringInFile(cmd, ARDO_PATH + "/configurations/standardList.config", true).empty())
    {
        return ARDO_PATH + "\\cmd\\standard\\" + cmd + "\\" + cmd + ".exe";
    }

    // checks if the command is inside the custom list. Builds the path if the command is found
    if (!isStringInFile(cmd, ARDO_PATH + "/configurations/customList.config", true).empty())
    {
        return ARDO_PATH + "\\cmd\\custom\\" + cmd + "\\" + cmd + ".exe";
    }

    // command could not be located ):
    std::cerr << "ERROR: Command '" + cmd + "' does not exist or isn't listed as a valid command/alias." << endl;
    return "";
}



// Will replace any command aliases with the corresponding command
void updateAliases(vector<string>& tokenizedInput)
{
    string alias = isStringInFile(tokenizedInput[0] + "->", ARDO_PATH + "/configurations/aliases.config", false );
    if (!alias.empty()) {
        std::size_t pos = alias.find("->");
        if (pos != std::string::npos) {
            tokenizedInput[0] = alias.substr(pos + 2); // skip "->"
            tokenizedInput[0].erase(0, tokenizedInput[0].find_first_not_of(" \t")); // Remove leading and trailing spaces
            tokenizedInput[0].erase(tokenizedInput[0].find_last_not_of(" \t") + 1);
        }
    }
}


void closeUpHandles(bool ownsReadHandle, bool ownsWriteHandle, HANDLE readHandle, HANDLE writeHandle, HANDLE errorHandle, bool closeWriteOnFinish) {
  if (ownsReadHandle) {
    drainPipe(readHandle);  // empties out the pipe so the program can continue as normal
    CloseHandle(readHandle);
  }
  if (ownsWriteHandle and closeWriteOnFinish) {
    FlushFileBuffers(writeHandle); // makes sure everything gets read from the write handle
    DisconnectNamedPipe(writeHandle);
    CloseHandle(writeHandle);
  }
  if (errorHandle != GetStdHandle(STD_ERROR_HANDLE)) { CloseHandle(errorHandle); }
}



// finds and executes the desired command
void commandHandler(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, HANDLE errorHandle, bool closeWriteOnFinish)
{
    bool ownsReadHandle = (readHandle != nullptr); // so we don't accidentally close stdin or stdout
    bool ownsWriteHandle = (writeHandle != nullptr);
    if (readHandle == nullptr) { readHandle = GetStdHandle(STD_INPUT_HANDLE); }
    if (writeHandle == nullptr) { writeHandle = GetStdHandle(STD_OUTPUT_HANDLE); }
    if (errorHandle == nullptr) { errorHandle = GetStdHandle(STD_ERROR_HANDLE); }

    embeddedHandler(tokenizedInput); // we first evaluate any embedded commands
    updateAliases(tokenizedInput);

    // handling and running builtin commands
    if (evaluateBuiltins(tokenizedInput, readHandle, writeHandle, errorHandle)) {
        closeUpHandles(ownsReadHandle, ownsWriteHandle, readHandle, writeHandle, errorHandle, closeWriteOnFinish);
        return;
    }

    // getting our command path. If no path is found, we return early.
    string commandFilePath = getCommandPath(tokenizedInput[0]);
    if (commandFilePath.empty() or !std::filesystem::exists(commandFilePath)) {
        if (!commandFilePath.empty()) {
            std::cerr << "ERROR: Command executable was not found. Expected path of executable: '" + commandFilePath + "'" << endl;
        }
        closeUpHandles(ownsReadHandle, ownsWriteHandle, readHandle, writeHandle, errorHandle, closeWriteOnFinish);
        return;
    }

    // setting up our command-line arguments to pass into the executable
    std::string cmdline = "\"" + commandFilePath + "\"";
    for (size_t i = 1; i < tokenizedInput.size(); i++) {
        cmdline += " " + tokenizedInput[i];
    }

    // we pass this into CreateProcess so windows knows where to redirect the input and output of the new process
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = readHandle;
    si.hStdOutput = writeHandle;
    si.hStdError = errorHandle;

    PROCESS_INFORMATION pi;

    // creating the new process
    if (!CreateProcess(NULL, cmdline.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed. Error: " << GetLastError() << "\n";
        closeUpHandles(ownsReadHandle, ownsWriteHandle, readHandle, writeHandle, errorHandle, closeWriteOnFinish);
        return;
    }

    // waiting for the new process to finish
    // TODO: add functionality that will terminate the running command if the user enters ctrl+k
    WaitForSingleObject(pi.hProcess, INFINITE);
    closeUpHandles(ownsReadHandle, ownsWriteHandle, readHandle, writeHandle, errorHandle, closeWriteOnFinish);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

}
