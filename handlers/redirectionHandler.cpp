#include "headers/redirectionHandler.h"
#include "headers/commandHandler.h"

// creates a file handle that the command will write to
// append is a boolean that dictates whether to append onto the given file or to erase it
HANDLE openFileHandle(const std::string& fileName, bool append) {

  SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };

  DWORD creation = append ? OPEN_ALWAYS : CREATE_ALWAYS;
  HANDLE h = CreateFileA(
      fileName.c_str(),
      GENERIC_WRITE,
      FILE_SHARE_READ,
      &saAttr,
      creation,
      FILE_ATTRIBUTE_NORMAL,
      NULL
  );

  if (h == INVALID_HANDLE_VALUE) {
    cerr << "ARDO ERROR: Failed to open file for output redirection: " << fileName << endl
         << "Will redirect to default destination. " << endl;
    return nullptr;
  }
  if (append) {
    SetFilePointer(h, 0, NULL, FILE_END);
  }
  return h;
}



// handles the logic for redirecting stdout and stderr
// remember, the last command in a pipe chain should have closeWriteOnFinish set to false so we don't close STD_OUTPUT_HANDLE
void redirectionHandler(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, bool closeWriteOnFinish)
{
  HANDLE stdoutHandle = writeHandle;
  HANDLE stderrHandle = GetStdHandle(STD_ERROR_HANDLE);
  vector<string> updatedCommand; // new command after we remove all the redirection syntax

  for (int i = 0; i < tokenizedInput.size(); i++) {
    string token = tokenizedInput[i];

    // if we are redirecting stdout
    if (token == ">" or token == ">>") {
      if (i+1 >= tokenizedInput.size()) {
        cerr << "ARDO ERROR: No file specified for output redirection after token '" << token << "'" << endl
             << "Will redirect to default destination. " << endl;
        continue;
      }

      // we need to check if this command was embedded into another, as the code that handles embedded commands
      // will need to have the final write end open so it doesn't crash when it tries to read from its pipe.
      // ('man grep' would be the embedded command in this example: 'echo $(man grep)' )
      if (stdoutHandle == writeHandle and closeWriteOnFinish and writeHandle != nullptr) {
        DisconnectNamedPipe(writeHandle);
        CloseHandle(writeHandle);
      }
      closeWriteOnFinish = false; // we set it to false as we will close the handle ourselves after the command finishes running

      string outputFile = tokenizedInput[i+1];
      if (stdoutHandle != writeHandle) { CloseHandle(stdoutHandle); }
      stdoutHandle = openFileHandle(outputFile, (token == ">>"));
      i++;
    }

    // if we are redirecting stderr
    else if (token == "2>" || token == "2>>") {
      if (i+1 >= tokenizedInput.size()) {
        cerr << "ARDO ERROR: No file specified for output redirection after token '" << token << "'" << endl
             << "command will run as normal without output redirection" << endl;
        continue;
      }
      string outputFile = tokenizedInput[i+1];
      if (stderrHandle != GetStdHandle(STD_ERROR_HANDLE)) { CloseHandle(stderrHandle); }
      stderrHandle = openFileHandle(outputFile, (token == "2>>"));
      i++;
    }

    // if we are redirecting both stdout and stderr
    else if (token == "&>" || token == "&>>") {
      if (i+1 >= tokenizedInput.size()) {
        cerr << "ARDO ERROR: No file specified for output redirection after token '" << token << "'" << endl
             << "command will run as normal without output redirection" << endl;
        continue;
      }

      if (stdoutHandle == writeHandle and closeWriteOnFinish and writeHandle != nullptr) {
        DisconnectNamedPipe(writeHandle);
        CloseHandle(writeHandle);
      }
      closeWriteOnFinish = false;

      string outputFile = tokenizedInput[i+1];
      if (stdoutHandle != writeHandle) { CloseHandle(stdoutHandle); }
      if (stderrHandle != GetStdHandle(STD_ERROR_HANDLE)) { CloseHandle(stderrHandle); }
      HANDLE outputHandle = openFileHandle(outputFile, (token == "&>>"));
      stdoutHandle = outputHandle;
      stderrHandle = outputHandle;
      i++;
    }

    // if the token isn't for redirection, we add it to a new array that contains the final command
    else {
      updatedCommand.push_back(tokenizedInput[i]);
    }
  }

  // pass in the new handles to our command handler
  commandHandler(updatedCommand, readHandle, stdoutHandle, stderrHandle, closeWriteOnFinish);

  if (stdoutHandle != writeHandle) { CloseHandle(stdoutHandle); }
  if (stderrHandle != GetStdHandle(STD_ERROR_HANDLE)) { CloseHandle(stderrHandle); }
}