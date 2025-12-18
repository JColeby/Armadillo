#include "headers/cmd.h"
#include "../../TerminalFormatting.h"
#include "../../commonFunctions/stripWhitespace.h"
#include "../../commonFunctions/handleIO.h"
#include "../../path.h"
#include <fstream>
#include <set>
#include <filesystem>

namespace fs = std::filesystem;

using HDL::writeToHandle;


using namespace VT;


struct Options {
  bool getBuiltin = false;
  bool getStandard = false;
  bool getCustom = false;
  int commandCount = 100;
};


bool evaluateFlags(vector<string> tokenizedInput, Options& opt) {
    for (int i = 0; i < tokenizedInput.size(); i++) {
        string param = tokenizedInput[i];
        if (!param.empty() and param[0] == '-') {
            for (int j = 1; j < param.size(); j++) {
                try {
                    switch (param[j]) {
                        case 'b': opt.getBuiltin = true; break;
                        case 's': opt.getStandard = true; break;
                        case 'c': opt.getCustom = true; break;
                        case 'a': opt.commandCount = 999999999; break;
                        case 'C': opt.commandCount = std::stoi(tokenizedInput[i + 1]); i++; break;
                        default: break;
                    }
                }
                catch (const std::out_of_range&) {
                  std::stringstream errorMessage;
                  errorMessage << "expected an additional parameter after flag -" << param[j];
                  throw std::runtime_error(errorMessage.str());
                }
                catch (const std::invalid_argument&) {
                  std::stringstream errorMessage;
                  errorMessage << "number expected. " << tokenizedInput[i+1] << "is not a number" << param[j];
                  throw std::runtime_error(errorMessage.str());
                }
            }
        }
        else {
          std::stringstream errorMessage;
          errorMessage << "additional parameters were provided when none were expected";
          throw std::runtime_error(errorMessage.str());
        }
    }
    if (!opt.getBuiltin and !opt.getStandard and !opt.getCustom) {
        opt.getBuiltin = true;
        opt.getStandard = true;
    }
    return true;
}



std::string removeExtension(const std::string& filename) {
  fs::path p(filename);
  return p.replace_extension("").string();
}


int cmdMain(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, HANDLE errorHandle) {
    Options opt;
    try { evaluateFlags(tokenizedInput, opt); }
    catch (std::runtime_error& e) {
      std::stringstream errorMessage;
      errorMessage << "SYNTAX ERROR: " << e.what() << endl;
      writeToHandle(errorHandle, errorMessage.str());
      return -1;
    }

    const char* white = "";
    const char* reset = "";

    if (verify(writeHandle)) {
      white = WHITE;
      reset = RESET_TEXT;
    }


    std::stringstream output;
    output << "Displaying the first " << opt.commandCount << " commands from the selected categories. To view more info about a specific command, run 'man <command>'." << endl;
    if (opt.getBuiltin) {
        std::set<string> commands;
        output << white << "Builtin Commands: " << reset;
        int itemsAdded = 0;
        for (const auto& entry : fs::directory_iterator(ARDO_PATH + "\\cmd\\builtinManuals\\")) {
          if (entry.is_regular_file()) {
            if (opt.commandCount < itemsAdded) { break; }
            if (itemsAdded != 0) { output << ", "; }
            output << removeExtension(entry.path().filename().string());
            itemsAdded++;
          }
        }
        output << endl;
    }

    if (opt.getStandard) {
        output << white << "Standard Commands: " << reset;
        int itemsAdded = 0;
        for (const auto& entry : fs::directory_iterator(ARDO_PATH + "\\cmd\\standard\\")) {
          if (entry.is_directory()) {
            if (opt.commandCount < itemsAdded) { break; }
            if (itemsAdded != 0) { output << ", "; }
            output << entry.path().filename().string();;
            itemsAdded++;
          }
        }
        output << endl;
    }

    if (opt.getCustom) {
        output << white << "Custom Commands: " << reset;
        int itemsAdded = 0;
        for (const auto& entry : fs::directory_iterator(ARDO_PATH + "\\cmd\\custom\\")) {
          if (entry.is_directory()) {
            if (opt.commandCount < itemsAdded) { break; }
            if (itemsAdded != 0) { output << ", "; }
            output << entry.path().filename().string();
            itemsAdded++;
          }
        }
        output << endl;
    }

    writeToHandle(writeHandle, output.str());
    return 0;
}