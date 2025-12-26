#include "headers/man.h"
#include "../../TerminalFormatting.h"
#include "../../commonFunctions/getTextFile.h"
#include "../../commonFunctions/isStringInFile.h"
#include "../../commonFunctions/handleIO.h"
#include "../../path.h"
#include <fstream>


using HDL::writeToHandle;
using HDL::writeToErrHandle;


using namespace VT;

int manMain(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, HANDLE errorHandle) {
    if (tokenizedInput.size() != 2) {
        writeToErrHandle(errorHandle, "SYNTAX ERROR: Incorrect number of arguments. Command syntax: man <command>\n");
        return -1;
    }

    string path = "";
    if (std::filesystem::exists(ARDO_PATH + "\\cmd\\builtinManuals\\" + tokenizedInput[1] + ".txt")) {
        path = ARDO_PATH + "\\cmd\\builtinManuals\\" + tokenizedInput[1] + ".txt";
    }
    else if (std::filesystem::exists(ARDO_PATH + "\\cmd\\custom\\" + tokenizedInput[1] + "\\" + tokenizedInput[1] + ".exe")){
      path = ARDO_PATH + "\\cmd\\custom\\" + tokenizedInput[1] + "\\manual.txt";
    }
    else if (std::filesystem::exists(ARDO_PATH + "\\cmd\\standard\\" + tokenizedInput[1] + "\\" + tokenizedInput[1] + ".exe")) {
        path = ARDO_PATH + "\\cmd\\standard\\" + tokenizedInput[1] + "\\manual.txt";
    }


    if (path.empty()) {
        writeToErrHandle(errorHandle, "ERROR: " + tokenizedInput[1] + " is not a valid command.\n");
        return -1;
    }
    if (!std::filesystem::exists(path)) {
        writeToErrHandle(errorHandle, "ERROR: manual file not found. Expected path: " + path + "\n");
        return -1;
    }

    writeToHandle(writeHandle, getTextFile(path));
    return 0;
}

