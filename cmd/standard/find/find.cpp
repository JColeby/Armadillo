#include <iostream>
#include <sstream>
#include <filesystem>
#include <windows.h>
#include "find.h"
#include "../../../commonFunctions/stringManipulation.h"
#include "../../../redCerr.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::cerr;

namespace fs = std::filesystem;


bool validateSyntaxAndSetFlags(std::vector<std::string> &tokenizedInput, Options& opt) {
    int nonFlagCount = 0;
    bool depthSet = false;
    for (size_t i = 1; i < tokenizedInput.size(); i++) { // looping through each parameter except the command name
        const std::string& param = tokenizedInput[i];

        // setting flag values
        if (param.size() > 1 && param[0] == '-' && !std::isdigit(param[1])) {
            std::string currentFlag; // holds a string of the flag we are evaluating so the user knows which flag caused an error
            try {
                // multi-character flags
                if (param[1] == '-') {
                    currentFlag = param;
                    if (param == "--depth") { opt.depth = std::stoi(tokenizedInput[i + 1]); i++; depthSet = true; }
                }
                // single letter flags
                else {
                    currentFlag = "-?";
                    for (int j = 1; j < param.size(); j++) {
                        currentFlag[1] = param[j];
                        switch (param[j]) {
                            // set boolean values for normal flags
                            case 'd': opt.matchDirectories = true; break;
                            case 'f': opt.matchFiles = true; break;
                            case 'a': opt.getAllMatches = true; break;
                            case 's': opt.displayRelativePath = true; break;
                            case 'r': opt.recursiveSearch = true; break;
                            case 'h': opt.searchHiddenDirectories = true; break;
                            case 'i': opt.notCaseSensitive = true; break;
                            case 'D': opt.depth = std::stoi(tokenizedInput[i + 1]); i++; depthSet = true; break;
                            default: std::cerr << "SYNTAX ERROR: Unexpected flag: -" << currentFlag << std::endl; return false;
                        }
                    }
                }
            }
            // catch the errors thrown from the statements above.
            catch (const std::out_of_range&) {
                std::cerr << "SYNTAX ERROR: expected additional parameter after flag " << currentFlag << std::endl;
                return false;
            }
            catch (const std::invalid_argument&) {
                std::cerr << "SYNTAX ERROR: number expected after flag " <<  currentFlag << ". " << tokenizedInput[i+1] << " is not a number" << std::endl;
                return false;
            }
            catch (...) {
                std::cerr << "SYNTAX ERROR: unexpected error occurred while interpreting flag " << currentFlag << std::endl;
                return false;
            }
        }
        else if (param.size() == 1 && param[0] == '-') { std::cerr << "SYNTAX ERROR: expected flag identifier after '-'" << std::endl; return false; }

        // setting normal Items
        else {
            switch (nonFlagCount) {
                case 0: opt.startingDirectory = param; break;
                case 1: opt.desiredFile = param; break;
                default: std::cerr << "SYNTAX ERROR: Too many arguments provided" << std::endl; return false;
            }
            nonFlagCount++;
        }
    }
    // final checks
    if (opt.startingDirectory.empty()) { std::cerr << "SYNTAX ERROR: expected additional parameter 'startingDirectory'" << std::endl; return false; }
    if (opt.desiredFile.empty()) { std::cerr << "SYNTAX ERROR: expected additional parameter 'desiredFile'" << std::endl; return false; }
    if (!opt.recursiveSearch && depthSet) { std::cerr << "WARNING: depth was set without the recursion flag present." << std::endl; }
    if (opt.recursiveSearch && !depthSet) { opt.recurseIndefinitely = true; }
    if (!opt.matchDirectories && !opt.matchFiles) { opt.matchFiles = true; }

    return true;
}


void setSearchInfo(Options& opt, SearchInfo& sInfo) {
    string check = opt.desiredFile;
    if (opt.notCaseSensitive) {
        STR_MANIP::toLower(check);
    }
    if (opt.desiredFile.front() == '*') {
        check.erase(0, 1);
        sInfo.starInFront = true;
    }
    if (opt.desiredFile.back() == '*') {
        check.erase(check.size() - 1);
        sInfo.starInBack = true;
    }
    sInfo.stringToFind = check;
}


// searches through files to find the desired file. Depth specifies how deep we will search
void searchDirectoryEntries(const fs::path& directoryPath, int currentDepth, Options& opt, SearchInfo& sInfo) {
    if (!opt.recurseIndefinitely && currentDepth < 0) { return; }
    for (const fs::directory_entry& entry : fs::directory_iterator(directoryPath, fs::directory_options::skip_permission_denied)) {
        int status = checkDirectoryEntry(entry, opt, sInfo);
        if (status == 1 && !opt.getAllMatches) { return; }
        // We recursively search the files if needed
        if (opt.recursiveSearch && entry.is_directory() && status != 2) {
            searchDirectoryEntries(entry.path(), currentDepth - 1, opt, sInfo);
        }
    }
}


// returns 0 if it didn't match, 1 if it did match, and 2 if the file or directory was hidden (unless -h flag was passed in)
int checkDirectoryEntry(const fs::directory_entry& entry, Options& opt, SearchInfo& sInfo) {
    const fs::path& entryPath = entry.path();
    std::string entryName = entryPath.filename().string();

    // skip hidden folders. returns 2 if a hidden file is found and the searchHiddenDirectories flag is set to false
    if (!opt.searchHiddenDirectories) {
        DWORD attributes = GetFileAttributesA(entryPath.string().c_str());
        bool hiddenByWindows = (attributes != INVALID_FILE_ATTRIBUTES) && (attributes & FILE_ATTRIBUTE_HIDDEN);
        bool hasPeriodPrefix = !entryName.empty() && entryName[0] == '.';
        if (hiddenByWindows || hasPeriodPrefix) { return 2; }
    }

    std::string currentDirectory;
    if (opt.displayRelativePath) { currentDirectory = entryPath.parent_path().lexically_relative(fs::absolute(opt.startingDirectory)).string(); }
    else { currentDirectory = fs::absolute(entryPath.parent_path()).string(); }

    if (entry.is_directory() && opt.matchDirectories) {
        if (checkEntryName(entryName, currentDirectory, true, opt, sInfo)) { return 1; }
    }
    else if (entry.is_regular_file() && opt.matchFiles) {
        if (checkEntryName(entryName, currentDirectory, false, opt, sInfo)) { return 1; }
    }

    return 0;
}


  // will return true if we found a match and the getAllMatches flag is false
  bool checkEntryName(string file, string currentDirectory, bool isDirectory, Options& opt, SearchInfo& sInfo) {
      if (opt.notCaseSensitive) { STR_MANIP::toLower(file); }

      if (sInfo.starInFront and sInfo.starInBack) {
        if (file.find(sInfo.stringToFind) != string::npos) {
            printFoundItem(currentDirectory, file, isDirectory);
            return true;
        }
      }
      else if (sInfo.starInFront) {
        if (STR_MANIP::endsWith(file, sInfo.stringToFind)) {
            printFoundItem(currentDirectory, file, isDirectory);
            return true;
        }
      }
      else if (sInfo.starInBack) {
        if (STR_MANIP::startsWith(file, sInfo.stringToFind)) {
            printFoundItem(currentDirectory, file, isDirectory);
            return true;
        }
      }
      else {
        if (sInfo.stringToFind == file) {
            printFoundItem(currentDirectory, file, isDirectory);
            return true;
        }
      }
      return false;
  }


  void printFoundItem(const string& currentDirectory, const string& file, bool isDirectory) {
    string newMatch = currentDirectory + "\\" + file;
    if (isDirectory) { newMatch += fs::path::preferred_separator; }
    cout << newMatch << endl;
  }


// custom parsing for command-line arguments because windows will automatically motify * syntax which we don't want
std::vector<std::string> splitArgs(const std::string& cmdline) {
    std::vector<std::string> args;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < cmdline.size(); ++i) {
        char c = cmdline[i];
        if (c == '"') { inQuotes = !inQuotes; }
        else if (c == ' ' && !inQuotes) {
            if (!current.empty()) {
                args.push_back(current);
                current.clear();
            }
        }
        else { current += c; }
    }
    if (!current.empty()) { args.push_back(current); }
    return args;
}


int main(int argc, char* argv[]) {
    setCerrColorRed();
    Options opt;
    std::vector<std::string> tokenizedInput = splitArgs(std::string(GetCommandLine()));
    if (!validateSyntaxAndSetFlags(tokenizedInput, opt)) { return EXIT_FAILURE; }

    SearchInfo sInfo;
    setSearchInfo(opt, sInfo);

    fs::path startPath = opt.startingDirectory;
    searchDirectoryEntries(startPath, opt.depth, opt, sInfo);
}
