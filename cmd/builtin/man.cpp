#include <fstream>
#include "headers/man.h"
#include "../../commonFunctions/writeToPipe.h"
#include "../../commonFunctions/isStringInFile.h"
#include "../../commonFunctions/getTextFile.h"
#include "../../TerminalFormatting.h"
#include "../../path.h"

using namespace VT;

int manMain(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle)
{
    if (tokenizedInput.size() != 2) {
        cerr << "ERROR: Incorrect number of arguments. Command syntax: man <command>" << endl;
        return -1;
    }

    string path = "";
    if (!isStringInFile(tokenizedInput[1], ARDO_PATH + "\\configurations\\builtinList.config", true).empty()) {
        path = ARDO_PATH + "\\cmd\\builtinManuals\\" + tokenizedInput[1] + ".txt";
    }
    if (!isStringInFile(tokenizedInput[1], ARDO_PATH + "\\configurations\\standardList.config", true).empty()) {
        path = ARDO_PATH + "\\cmd\\standard\\" + tokenizedInput[1] + "\\manual.txt";
    }
    if (!isStringInFile(tokenizedInput[1], ARDO_PATH + "\\configurations\\customList.config", true).empty()){
        path = ARDO_PATH + "\\cmd\\custom\\" + tokenizedInput[1] + "\\manual.txt";
    }

    if (path.empty()) {
        cerr << "ERROR: Not a valid command name." << endl;
        return -1;
    }
    if (!std::filesystem::exists(path)) {
        cerr << "ERROR: manual file not found. Expected path: " << path << endl;
        return -1;
    }

    writeToPipe(writeHandle, getTextFile(path));
    return 0;
}