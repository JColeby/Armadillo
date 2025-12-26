#include "headers/cd.h"
#include "../../commonFunctions/handleIO.h"


using HDL::writeToErrHandle;


namespace fs = std::filesystem;

int cdMain(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, HANDLE errorHandle)
{
    if (tokenizedInput.size() == 1) {
        writeToErrHandle(errorHandle, "SYNTAX ERROR: Expected directory path\n");
        return -1;
    }
    if (tokenizedInput.size() > 2) {
        std::stringstream errorMessage;
        errorMessage << "SYNTAX ERROR: Too many arguments." << endl
             << "If the directory you are trying to change to contains a space, please surround it with quotes." << endl
             << "Example: cd \"folder with spaces\" ";
        writeToErrHandle(errorHandle, errorMessage.str());
        return -1;
    }

    try {
        std::string path = tokenizedInput[1];
        if (path.size() >= 2 && path.front() == '"' && path.back() == '"') { // removes quotes from the path if they exist
            path = path.substr(1, path.size() - 2);
        }
        fs::current_path(path);
        return 0;
    }
    catch (...) {
        writeToErrHandle(errorHandle, "ERROR: No such file or directory\n");
        return -1;
    }
}