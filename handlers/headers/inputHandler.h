#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <windows.h>
#include <thread>
#include <chrono>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::vector;
using std::cerr;


void inputHandler(const string& userInput);

void printBadCommandResult(const vector<string>& commandOutput);

void displayOutput(const vector<string>& commandOutput);

vector<string> tokenizeInput(const string& inputString, bool removeQuotes);









