#include <fstream>
#include "headers/alias.h"
#include "../../commonFunctions/isStringInFile.h"
#include "../../commonFunctions/writeToPipe.h"
#include "../../path.h"

int aliasMain(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, HANDLE errorHandle)
{
    if (tokenizedInput.size() != 3) {
        writeToPipe(errorHandle, "ERROR: Incorrect number of arguments. Command syntax: alias <newAlias> <command>\n");
        return -1;
    }

    // checking if the command exists
    if (!isStringInFile(tokenizedInput[2], ARDO_PATH + "/configurations/builtinList.config", true).empty() or
        !isStringInFile(tokenizedInput[2], ARDO_PATH + "/configurations/standardList.config", true).empty() or
        !isStringInFile(tokenizedInput[2], ARDO_PATH + "/configurations/customList.config", true).empty())
    {
        std::ofstream file(ARDO_PATH + "/configurations/aliases.config", std::ios::app);
        if (!file) {
            writeToPipe(errorHandle, "ERROR: Failed to open aliases.config\n  expected path: " + ARDO_PATH + "/configurations/aliases.config\n");
            return -1;
        }
        file << endl << tokenizedInput[1] << "->" << tokenizedInput[2];
        return 0;
    }

    writeToPipe(errorHandle, "ERROR: command doesn't exist or isn't listed as a valid command.");
    return -1;
}