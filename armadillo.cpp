#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <filesystem>

#include "TerminalFormatting.h"
#include "path.h"
#include "commonFunctions/getTextFile.h"
#include "handlers/headers/inputHandler.h"

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;

using namespace VT;


// Makes sure Ardo can find all the essential files before starting.
// Will refactor this later so there isn't as much duplicate code
int validatePathStructure() {
    std::stringstream missing;
    if (!std::filesystem::exists(ARDO_PATH)) {
        std::cerr << "FATAL ERROR: Could not find '" + ARDO_PATH + "'. Did you modify 'path.h' or 'build.bat'?";
        return -1;
    }
    if (!std::filesystem::exists(ARDO_PATH + "/cmd/standard")) {
        missing << "  'cmd\\standard'" << endl;
    }
    if (!std::filesystem::exists(ARDO_PATH + "/cmd/custom")) {
        missing << "  'cmd\\custom'" << endl;
    }
    if (!std::filesystem::exists(ARDO_PATH + "/configurations/aliases.config")) {
        missing << "  'configurations\\aliases.config'" << endl;
    }
    if (!std::filesystem::exists(ARDO_PATH + "/configurations/customList.config")) {
        missing << "  'configurations\\customList.config'" << endl;
    }
    if (!std::filesystem::exists(ARDO_PATH + "/configurations/standardList.config")) {
        missing << "  'configurations\\standardList.config'" << endl;
    }

    string missingPaths = missing.str();
    if (!missingPaths.empty()) {
        std::cerr << "FATAL ERROR: The following critical files/directories could not be found within the main path:" << endl
                  << missingPaths << "Main path: " << ARDO_PATH << endl
                  << "Did you modify the directory or 'build.bat'?" << endl;
        return -1;
    }
    return 1;
}


// displays the logo and ASCII art that you see at the start of the program
void displayLogoAndASCII()
{
    system("cls");

    if (std::filesystem::exists(ARDO_PATH + "/resources/armadilloASCII.txt")) {
        string armadilloASCII = getTextFile(ARDO_PATH + "/resources/armadilloASCII.txt");
        cout << YELLOW << armadilloASCII << RESET_TEXT;
    } else {
        cout << endl << RED << "ERROR: Failed to display ASCII art. '" << ARDO_PATH << "/resources/armadilloASCII.txt' does not exist." << endl << RESET_TEXT;
    }

    cout << "##########################################################################################";

    if (std::filesystem::exists(ARDO_PATH + "/resources/armadilloASCII.txt")) {
        string armadilloLogo = getTextFile(ARDO_PATH + "/resources/armadilloLogo.txt");
        cout << BLUE << armadilloLogo << RESET_TEXT;
    } else {
        cout << endl << RED << "ERROR: Failed to display Logo. '" << ARDO_PATH << "/resources/armadilloLogo.txt' does not exist." << endl << RESET_TEXT;
    }

    cout << "##########################################################################################" << endl << endl;

    cout << WHITE;
    cout << "     -  run 'help' for additional information about Armadillo" << endl << endl;
}

// shows the working directory. Shown right before user prompt
int displayCurrentDirectory() {
    TCHAR pathBuffer[MAX_PATH];
    DWORD length = GetCurrentDirectory(MAX_PATH, pathBuffer); // windows system call
    if (length == 0) {
        std::cerr << "FATAL ERROR: failed to fetch current directory. Exiting Armadillo" << endl;
        return -1;
    }
    cout << BLUE << "ARDO"
         << WHITE  << " => "
         << CYAN << pathBuffer
         << WHITE << " => "
         << DEFAULT;
    return 1;
}

// main command loop.
int inputLoop() {
    HANDLE finalWriteHandle = nullptr;
    while (true) {
        if (displayCurrentDirectory() == -1) { return -1; }
        string input;
        getline(cin, input);
        if (input == "exit") { return 0; }
        inputHandler(input, finalWriteHandle);
        // runningCommand = true;
        // thread t(inputHandler, input);
        // while (runningCommand) {
        //     bool ctrlDown = GetAsyncKeyState(VK_CONTROL) & 0x8000; // 0x8000 means the key is currently being pressed
        //     bool qDown = GetAsyncKeyState('Q') & 0x8000;
        //     if (ctrlDown && qDown) { killSwitch = true; }
        // }
        // t.join();
        // killSwitch = false;
    }
}



int main() {
    // this allows us to make fancy terminal output by enabling ENABLE_VIRTUAL_TERMINAL_PROCESSING
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD originalMode = 0;
    GetConsoleMode(hOut, &originalMode);
    DWORD newMode = originalMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, newMode);

    // validating that we have all the essential files
    int pathValidation = validatePathStructure();
    if (pathValidation == -1) {
        SetConsoleMode(hOut, originalMode);
        return -1;
    }

    displayLogoAndASCII();

    int exitCode = inputLoop(); // going to the main loop
    if (exitCode == 0) { system("cls"); } // clears the screen if exited properly
    SetConsoleMode(hOut, originalMode);
    CloseHandle(hOut);
    return exitCode;
}

