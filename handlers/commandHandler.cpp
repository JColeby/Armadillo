#include "headers/commandHandler.h"
#include "headers/embeddedHandler.h"
#include "../path.h"
#include "../commonFunctions/isStringInFile.h"
#include "../cmd/builtin/headers/cd.h"
#include "../cmd/builtin/headers/alias.h"
#include "../cmd/builtin/headers/help.h"
#include "../cmd/builtin/headers/cmd.h"
#include "../cmd/builtin/headers/man.h"


// evaluates and executes builtin commands
bool evaluateBuiltins(vector<string>& tokenizedInput, HANDLE readHandle, HANDLE writeHandle)
{
    try {
        string command = tokenizedInput[0];
        if (command == "clear") {  system("cls"); return true; }
        if (command == "cd") { cdMain(tokenizedInput, readHandle, writeHandle); return true; }
        if (command == "alias") { aliasMain(tokenizedInput, readHandle, writeHandle); return true; }
        if (command == "help") { helpMain(tokenizedInput, readHandle, writeHandle); return true; }
        if (command == "cmd") { cmdMain(tokenizedInput, readHandle, writeHandle); return true; }
        if (command == "man") { manMain(tokenizedInput, readHandle, writeHandle); return true; }
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



// finds and executes the desired command
void commandHandler(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, bool closeWriteOnFinish)
{
    bool ownsReadHandle = (readHandle != nullptr); // so we don't accidentally close stdin or stdout
    bool ownsWriteHandle = (writeHandle != nullptr);
    if (readHandle == nullptr) { readHandle = GetStdHandle(STD_INPUT_HANDLE); }
    if (writeHandle == nullptr) { writeHandle = GetStdHandle(STD_OUTPUT_HANDLE); }

    embeddedHandler(tokenizedInput);
    updateAliases(tokenizedInput);

    // handling and running builtin commands
    if (evaluateBuiltins(tokenizedInput, readHandle, writeHandle))
    {
        if (ownsReadHandle) { CloseHandle(readHandle); }
        if (ownsWriteHandle and closeWriteOnFinish) { CloseHandle(writeHandle); }
        return;
    }

    // getting our command path. If no path is found, we return early.
    string commandFilePath = getCommandPath(tokenizedInput[0]);
    if (commandFilePath.empty() or !std::filesystem::exists(commandFilePath)) {
        if (!commandFilePath.empty()) {
            std::cerr << "ERROR: Command executable was not found. Expected path of executable: '" + commandFilePath + "'" << endl;
        }
        if (ownsReadHandle) { CloseHandle(readHandle); }
        if (ownsWriteHandle and closeWriteOnFinish) { CloseHandle(writeHandle); }
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
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE); // keeping this the same so the user knows about errors.

    PROCESS_INFORMATION pi;

    // creating the new process
    if (!CreateProcess(NULL, cmdline.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed. Error: " << GetLastError() << "\n";
        if (ownsReadHandle) { CloseHandle(readHandle); }
        if (ownsWriteHandle and closeWriteOnFinish) { CloseHandle(writeHandle); }
        return;
    }

    // waiting for the new process to finish
    // TODO: add functionality that will terminate the running command if the user enters ctrl+k
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);


    if (ownsReadHandle) { CloseHandle(readHandle); }
    if (ownsWriteHandle and closeWriteOnFinish) { CloseHandle(writeHandle); }

}
