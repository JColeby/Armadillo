#include "headers/pipeHandler.h"
#include "headers/redirectionHandler.h"
#include "../commonFunctions/separateTokensByToken.h"
#include "../commonFunctions/handleIO.h"


using HDL::makePipe;



// creates a pipe and passes in the read+write handles to commandHandler, which will execute the corresponding commands
void pipeHandler(const vector<string>& tokenizedInput, HANDLE& finalWriteHandle) {
  vector<vector<string>> individualCommands = separateTokensByToken(tokenizedInput, "|");

  // if we don't have to pipe the output
  if (individualCommands.size() == 1) { redirectionHandler(individualCommands[0], nullptr, finalWriteHandle, false); return; }


  std::vector<std::thread> threads;
  HANDLE previousReadHandle = nullptr; // handle that the next command will read from

  for (int i = 0; i < individualCommands.size() - 1; i++) {
    HANDLE writeHandle, readHandle;
    makePipe(writeHandle, readHandle);

    // create a new thread that calls commandHandler, which will create the new processes and set their stdin and stdout handles
    threads.emplace_back([cmd = individualCommands[i], readH = previousReadHandle, writeH = writeHandle]() {
      redirectionHandler(cmd, readH, writeH, true);
    });

    previousReadHandle = readHandle; // next command will now be able to read from this handle
  }

  // the very last thread should write to stdout (or to another handle if it's an embedded command)
  // we want to close the write handle on finish for our pipes but not the final handle, as that would either break the embedded command or close STD_OUTPUT_HANDLE
  threads.emplace_back(redirectionHandler, individualCommands[individualCommands.size()-1], previousReadHandle, finalWriteHandle, false);

  for (auto& t : threads) {
    t.join();
  }

}