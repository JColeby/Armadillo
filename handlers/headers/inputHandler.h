#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <windows.h>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::cerr;


std::vector<std::string> tokenizeInput(const std::string& inputString, bool removeQuotes);

void inputHandler(const std::string& userInput, HANDLE& finalWriteHandle);













