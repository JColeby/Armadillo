#pragma once
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace fs = std::filesystem;

struct SearchInfo {
    bool starInFront = false;
    bool starInBack = false;
    string stringToFind;
};

struct Options {
    bool matchDirectories = false;          // -d
    bool matchFiles = false;                // -f
    bool getAllMatches = false;             // -a
    bool displayRelativePath = false;       // -s   displays relative path
    bool recursiveSearch = false;           // -r
    bool searchHiddenDirectories = false;   // -h
    bool notCaseSensitive = false;          // -i
    bool recurseIndefinitely = false;       // set to true unless depth is set
    int depth = 0;                          // -D   Requires a number directly after this flag
    string startingDirectory;
    string desiredFile;
};

bool validateSyntaxAndSetFlags(std::vector<std::string> &tokenizedInput, Options& opt);

void setSearchInfo(Options& opt, SearchInfo& sInfo);

void searchDirectoryEntries(const fs::path& directoryPath, int currentDepth, Options& opt, SearchInfo& sInfo);

int checkDirectoryEntry(const fs::directory_entry& entry, Options& opt, SearchInfo& sInfo);

bool checkEntryName(string file, string currentDirectory, bool isDirectory, Options& opt, SearchInfo& sInfo);

void printFoundItem(const string& currentDirectory, const string& file, bool isDirectory);

std::vector<std::string> splitArgs(const std::string& cmdline);

