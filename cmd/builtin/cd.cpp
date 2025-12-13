#include "headers/cd.h"

namespace fs = std::filesystem;

int cdMain(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle)
{
    if (tokenizedInput.size() == 1) {
        cerr << "ERROR: Please pass in the name of the directory you want to change to" << endl;
        return -1;
    }
    if (tokenizedInput.size() > 2) {
        cerr << "ERROR: More than 1 argument detected." << endl
             << "If the directory you are trying to change to contains a space, please surround it with quotes." << endl
             << "Example: cd \"folder with spaces\" ";
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
        cerr << "ERROR: No such file or directory" << endl;
        return -1;
    }
}