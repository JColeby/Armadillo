#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <windows.h>
#include <filesystem>
#include <fstream>
#include <unordered_set>

using std::string;
using std::vector;
using std::cout;
using std::endl;

void commandHandler(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, bool closeWriteOnFinish);