#include "headers/help.h"
#include "../../TerminalFormatting.h"
#include "../../commonFunctions/handleIO.h"
#include <fstream>

using HDL::writeToHandle;

using namespace VT;

int helpMain(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, HANDLE errorHandle)
{
    std::stringstream helpText;
    helpText << "  - press ctrl+k to terminate any active command (may result in memory leaks)" << endl;
    helpText << "  - run 'exit' to close the shell" << endl;
    helpText << "  - run 'cmd' to view a list of all commands" << endl;
    helpText << "  - run 'man <command>' to view additional info about a specific command" << endl;
    helpText << "  - to redirect output to a file, add '<redirectMode> <filepath>' to the end of your command" << endl;
    helpText << "  - here is a list of all the available redirect modes: " << endl;
    helpText << "      - '>' will write the contents of stdout to the file" << endl;
    helpText << "      - '>>' will append the contents of stdout to the file " << endl;
    helpText << "      - '2>' will write the contents of stderr to the file" << endl;
    helpText << "      - '2>>' will append the contents of stderr to the file " << endl;
    helpText << "      - '&>' will write the contents of stdout and stderr to the file " << endl;
    helpText << "      - '&>>' will append the contents of stdout and stderr to the file " << endl;
    helpText << "  - a pipe '|' can be used to redirect the output of one command into the input of another" << endl;
    helpText << "  - to pass the result of 1 command as a parameter to another one, surround the first command like so: '<cmd2> $(<cmd1>)'" << endl;

    string output = helpText.str();
    writeToHandle(writeHandle, output);
    return 0;
}