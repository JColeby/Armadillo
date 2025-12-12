#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <windows.h>
#include <thread>

using std::string;
using std::vector;
using std::cout;
using std::endl;

void pipeHandler(const vector<string>& tokenizedInput, HANDLE& finalWriteHandle);


