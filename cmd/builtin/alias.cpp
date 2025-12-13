#include <fstream>
#include "headers/alias.h"
#include "../../commonFunctions/isStringInFile.h"
#include "../../path.h"

int aliasMain(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle)
{
    if (tokenizedInput.size() != 3) {
        cerr << "ERROR: Incorrect number of arguments. Command syntax: alias <newAlias> <command>" << endl;
        return -1;
    }

    // checking if the command exists
    if (!isStringInFile(tokenizedInput[2], ARDO_PATH + "/configurations/builtinList.config", true).empty() or
        !isStringInFile(tokenizedInput[2], ARDO_PATH + "/configurations/standardList.config", true).empty() or
        !isStringInFile(tokenizedInput[2], ARDO_PATH + "/configurations/customList.config", true).empty())
    {
        std::ofstream file(ARDO_PATH + "/configurations/aliases.config", std::ios::app);
        if (!file) {
            std::cerr << "ERROR: Failed to open aliases.config" << endl << "expected path: " << ARDO_PATH << "/configurations/aliases.config" << endl;
            return -1;
        }
        file << endl << tokenizedInput[1] << "->" << tokenizedInput[2];
        return 0;
    }

    std::cerr << "ERROR: command doesn't exist or isn't listed as a valid command." << endl;
    return -1;
}