#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <windows.h>
#include <filesystem>
#include <fstream>

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

int cmdMain(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, HANDLE errorHandle);