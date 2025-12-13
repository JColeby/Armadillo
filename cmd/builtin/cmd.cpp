#include <fstream>
#include <set>
#include <algorithm>
#include "headers/cmd.h"
#include "../../commonFunctions/writeToPipe.h"
#include "../../commonFunctions/stripWhitespace.h"
#include "../../TerminalFormatting.h"
#include "../../path.h"

using namespace VT;

bool getBuiltin;
bool getStandard;
bool getCustom;
int commandCount;



void evaluateFlags(vector<string> tokenizedInput)
{
    getBuiltin = false;
    getStandard = false;
    getCustom = false;
    commandCount = 100;
    for (int i = 0; i < tokenizedInput.size(); i++) {
        string param = tokenizedInput[i];
        if (!param.empty() and param[0] == '-') {
            for (int j = 1; j < param.size(); j++) {
                switch (param[j]) {
                    case 'b': getBuiltin = true; break;
                    case 's': getStandard = true; break;
                    case 'c': getCustom = true; break;
                    case 'a': commandCount = 999999999; break;
                    case 'C':
                        {
                            try { commandCount = std::stoi(tokenizedInput[i + 1]); i++; break; }
                            catch (...) { throw std::runtime_error("ERROR: expected a number as next parameter after flag -C"); }
                        }
                    default: break;
                }
            }
        }
    }
    if (!getBuiltin and !getStandard and !getCustom) {
        getBuiltin = true;
        getStandard = true;
    }
}


void readAndAddToSet(std::set<string>& commands, string commandFile)
{
    std::ifstream file(commandFile);
    std::string line;
    while (std::getline(file, line)) {
        // Remove all whitespace characters
        stripWhitespace(line);
        if (line.empty()) { continue; }
        commands.insert(line);
    }
}


int cmdMain(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle)
{
    try { evaluateFlags(tokenizedInput); }
    catch (std::runtime_error& e) { cerr << e.what() << endl; return -1; }

    std::set<string> commands;
    std::stringstream output;
    output << "Displaying the first " << commandCount << " commands in each category. To view more info about a specific command, run 'man <command>'." << endl;
    if (getBuiltin) {
        readAndAddToSet(commands, ARDO_PATH + "/configurations/builtinList.config");
        output << WHITE << "Builtin Commands: " << RESET_TEXT;
        int itemsAdded = 0;
        for (const auto& value : commands) {
            if (commandCount < itemsAdded) { break; }
            if (itemsAdded != 0) { output << ", "; }
            output << value;
            itemsAdded++;
        }
        commands.clear();
        output << endl;
    }

    if (getStandard) {
        readAndAddToSet(commands, ARDO_PATH + "/configurations/standardList.config");
        output << WHITE << "Standard Commands: " << RESET_TEXT;
        int itemsAdded = 0;
        for (const auto& value : commands) {
            if (commandCount < itemsAdded) { break; }
            if (itemsAdded != 0) { output << ", "; }
            output << value;
            itemsAdded++;
        }
        commands.clear();
        output << endl;
    }

    if (getCustom) {
        readAndAddToSet(commands, ARDO_PATH + "/configurations/customList.config");
        output << WHITE << "Custom Commands: " << RESET_TEXT;
        int itemsAdded = 0;
        for (const auto& value : commands) {
            if (commandCount < itemsAdded) { break; }
            if (itemsAdded != 0) { output << ", "; }
            output << value;
            itemsAdded++;
        }
        commands.clear();
        output << endl;
    }

    writeToPipe(writeHandle, output.str());
    return 0;
}