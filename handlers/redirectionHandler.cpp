#include "headers/redirectionHandler.h"
#include "../commonFunctions/handleIO.h"
#include "headers/commandHandler.h"

using HDL::openInputFileHandle;
using HDL::openWriteFileHandle;
using HDL::drainPipe;

// handles the logic for redirecting stdout and stderr
// remember, the last command in a pipe chain should have closeWriteOnFinish set to false so we don't close STD_OUTPUT_HANDLE
void redirectionHandler(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, bool closeWriteOnFinish) {
  std::unordered_set<HANDLE> handlesToClose;

  HANDLE stdinHandle = readHandle;
  HANDLE stdoutHandle = writeHandle;
  HANDLE stderrHandle = GetStdHandle(STD_ERROR_HANDLE);
  vector<string> updatedCommand; // new command after we remove all the redirection syntax

  for (int i = 0; i < tokenizedInput.size(); i++) {
    string token = tokenizedInput[i];

    // if we are redirecting stdout
    if (token == ">" or token == ">>") {
      if (i+1 >= tokenizedInput.size()) {
        cerr << "ARDO ERROR: No file specified for output redirection after token '" << token << "'" << endl
             << "  output redirection has been ignored" << endl;
        continue;
      }

      string outputFile = tokenizedInput[i+1];
      HANDLE outputHandle;
      if (!openWriteFileHandle(outputHandle, outputFile, (token == ">>"))) {
        cerr << "ARDO ERROR: Failed to open file for output redirection: " << outputFile << endl
         << "  Will redirect stdout to previous destination. " << endl;
      }
      else if (outputHandle != nullptr) {
        // we need to check if this command was embedded into another, as the code that handles embedded commands
        // will need to have the final write end open so it doesn't crash when it tries to read from its pipe.
        // ('man grep' would be the embedded command in this example: 'echo $(man grep)' )
        if (stdoutHandle == writeHandle and closeWriteOnFinish and writeHandle != nullptr) {
          DisconnectNamedPipe(writeHandle);
          CloseHandle(writeHandle);
        }
        closeWriteOnFinish = false; // we set it to false as we will close the handle ourselves after the command finishes running

        if (stdoutHandle != writeHandle && stdoutHandle != nullptr) { handlesToClose.insert(stdoutHandle); }
        stdoutHandle = outputHandle;
      }
      i++;
    }

    // if we are redirecting stderr
    else if (token == "2>" || token == "2>>") {
      if (i+1 >= tokenizedInput.size()) {
        cerr << "ARDO ERROR: No file specified for output redirection after token '" << token << "'" << endl
             << "  output redirection has been ignored" << endl;
        continue;
      }
      string outputFile = tokenizedInput[i+1];
      HANDLE outputHandle;
      if (!openWriteFileHandle(outputHandle, outputFile, (token == "2>>"))) {
        cerr << "ARDO ERROR: Failed to open file for output redirection: " << outputFile << endl
         << "  Will redirect stderr to previous destination. " << endl;
      }
      else if (outputHandle != nullptr) {
        if (stderrHandle != GetStdHandle(STD_ERROR_HANDLE)) { handlesToClose.insert(stderrHandle); }
        stderrHandle = outputHandle;
      }
      i++;
    }

    // if we are redirecting both stdout and stderr
    else if (token == "&>" || token == "&>>") {
      if (i+1 >= tokenizedInput.size()) {
        cerr << "ARDO ERROR: No file specified for output redirection after token '" << token << "'" << endl
             << "  output redirection has been ignored" << endl;
        continue;
      }
      string outputFile = tokenizedInput[i+1];
      HANDLE outputHandle;
      if (!openWriteFileHandle(outputHandle, outputFile, (token == "&>>"))) {
        cerr << "ARDO ERROR: Failed to open file for output redirection: " << outputFile << endl
         << "Will redirect stdout and stderr to previous destination. " << endl;
      }
      else if (outputHandle != nullptr) {

        if (stdoutHandle == writeHandle and closeWriteOnFinish and writeHandle != nullptr) {
          DisconnectNamedPipe(writeHandle);
          CloseHandle(writeHandle);
        }
        closeWriteOnFinish = false;

        if (stdoutHandle != writeHandle && stdoutHandle != nullptr) { handlesToClose.insert(stdoutHandle); }
        if (stderrHandle != GetStdHandle(STD_ERROR_HANDLE)) { handlesToClose.insert(stderrHandle); }

        stdoutHandle = outputHandle;
        stderrHandle = outputHandle;
      }
      i++;
    }

    // if we are redirecting stdin
    else if (token == "<") {
      if (i + 1 >= tokenizedInput.size()) {
        cerr << "ARDO ERROR: No file specified for input redirection after token '" << token << "'" << endl
             << "  input redirection has been ignored" << endl;
        continue;
      }
      string inputFile = tokenizedInput[i + 1];
      HANDLE inputHandle;

      if (!openInputFileHandle(inputHandle, inputFile)) {
        cerr << "ARDO ERROR: Failed to open file for input redirection: " << inputFile << endl
         << "  Will pull stdin from previous destination. " << endl;
      }

      else if (inputHandle != nullptr) {
        if (stdinHandle != nullptr) {
          if (stdinHandle == readHandle) {
            // Close pipe read end so writer sees EOF
            CloseHandle(readHandle);
          } else {
            drainPipe(stdinHandle);
            handlesToClose.insert(stdinHandle);
          }
        }

        stdinHandle = inputHandle;
      }
      i++;
    }

    // if the token isn't for redirection, we add it to a new array that contains the final command
    else {
      updatedCommand.push_back(tokenizedInput[i]);
    }
  }

  // pass in the new handles to our command handler
  commandHandler(updatedCommand, stdinHandle, stdoutHandle, stderrHandle, closeWriteOnFinish);

  if (stdinHandle != readHandle) { handlesToClose.insert(stdinHandle); }
  if (stdoutHandle != writeHandle) { handlesToClose.insert(stdoutHandle); }
  if (stderrHandle != GetStdHandle(STD_ERROR_HANDLE)) { handlesToClose.insert(stderrHandle); }

  for (HANDLE h : handlesToClose) {
    CloseHandle(h);
  }
}
