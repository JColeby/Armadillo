#include "headers/cd.h"
#include "../../commonFunctions/writeToPipe.h"


namespace fs = std::filesystem;

int cdMain(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, HANDLE errorHandle)
{
    if (tokenizedInput.size() == 1) {
        writeToPipe(errorHandle, "ERROR: Please pass in the name of the directory you want to change to\n");
        return -1;
    }
    if (tokenizedInput.size() > 2) {
        std::stringstream errorMessage;
        errorMessage << "ERROR: More than 1 argument detected." << endl
             << "If the directory you are trying to change to contains a space, please surround it with quotes." << endl
             << "Example: cd \"folder with spaces\" ";
        writeToPipe(errorHandle, errorMessage.str());
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
        writeToPipe(errorHandle, "ERROR: No such file or directory\n");
        return -1;
    }
}