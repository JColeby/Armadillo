#include "headers/echo.h"
#include "../../commonFunctions/handleIO.h"
#include "../../commonFunctions/removeQuotes.h"

using HDL::writeToHandle;

int echoMain(vector<string> tokenizedInput, HANDLE readHandle, HANDLE writeHandle, HANDLE errorHandle)
{
  if (tokenizedInput.size() == 1) {
    writeToHandle(errorHandle, "SYNTAX ERROR: Expected at least 1 additional parameter\n");
    return -1;
  }
  for (size_t i = 1; i < tokenizedInput.size(); i++) {
    removeQuotes(tokenizedInput[i]);
    writeToHandle(writeHandle, tokenizedInput[i] + "\n");
  }
  return 0;
}