#include "headers/alias.h"
#include "../../commonFunctions/isStringInFile.h"
#include "../../commonFunctions/handleIO.h"
#include "../../path.h"
#include <fstream>

// using HDL::writeToHandle;
using HDL::writeToErrHandle;

int aliasMain(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, HANDLE errorHandle)
{
    if (tokenizedInput.size() != 3) {
        writeToErrHandle(errorHandle, "SYNTAX ERROR: Incorrect number of arguments. Command syntax: alias <newAlias> <command>\n");
        return -1;
    }
    // checking if the command exists
    if (std::filesystem::exists(ARDO_PATH + "\\cmd\\builtinManuals\\" + tokenizedInput[2] + ".txt") or
        std::filesystem::exists(ARDO_PATH + "\\cmd\\custom\\" + tokenizedInput[2] + "\\" + tokenizedInput[2] + ".exe") or
        std::filesystem::exists(ARDO_PATH + "\\cmd\\standard\\" + tokenizedInput[2] + "\\" + tokenizedInput[2] + ".exe"))
    {
        std::ofstream file(ARDO_PATH + "/configurations/aliases.config", std::ios::app);
        if (!file) {
            writeToErrHandle(errorHandle, "ERROR: Failed to open aliases.config\n  expected path: " + ARDO_PATH + "/configurations/aliases.config\n");
            return -1;
        }
        file << endl << tokenizedInput[1] << "->" << tokenizedInput[2];
        return 0;
    }

    writeToErrHandle(errorHandle, "ERROR: command doesn't exist or isn't listed as a valid command.");
    return -1;
}