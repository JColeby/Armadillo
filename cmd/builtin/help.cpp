#include <fstream>
#include "headers/help.h"
#include "../../commonFunctions/writeToPipe.h"
#include "../../TerminalFormatting.h"

using namespace VT;

int helpMain(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, HANDLE errorHandle)
{
    std::stringstream helpText;
    helpText << WHITE << "  - press ctrl+k to terminate any active command (may result in memory leaks)" << endl;
    helpText << "  - run 'exit' to close the shell" << endl;
    helpText << "  - run 'cmd' to view a list of all commands" << endl;
    helpText << "  - run 'man <command>' to view additional info about a specific command" << RESET_TEXT << endl;
    string output = helpText.str();
    writeToPipe(writeHandle, output);
    return 0;
}