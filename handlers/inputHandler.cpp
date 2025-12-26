#include "headers/inputHandler.h"
#include "headers/pipeHandler.h"
#include "../commonFunctions/separateTokensByToken.h"
#include "headers/commandHandler.h"

vector<string> tokenizeInput(const string& inputString, bool dontTokenizeEmbedded) {
    std::istringstream wordSeparator(inputString);
    vector<string> tokens;
    string token;
    // separating characters by whitespace
    while (wordSeparator >> token) {
        if (token.empty()) { continue; }

        // logic for embedded input.
        if (dontTokenizeEmbedded and token.size() >= 2 and token.rfind("$(", 0) == 0) {
            int parenthesesCount = 0;
            string embeddedCommand;
            do { // gets next token until all parentheses are closed
                for (char c : token) {
                    if (c == '(') parenthesesCount++;
                    if (c == ')') parenthesesCount--;
                }
                embeddedCommand += token + " ";
            } while (parenthesesCount != 0 and wordSeparator >> token);
            if (parenthesesCount != 0) {
                std::cerr << "ERROR: unmatched parentheses in embedded command.\n";
                return {};
            }
            embeddedCommand.pop_back(); // removes the additional whitespace that gets added to the end because of the loop
            tokens.push_back(embeddedCommand);
        }


        // logic for quoted input.
        else if (!token.empty() && token[0] == '"') {
            string quotedToken;
            do { // gets next token until we hit a word that ends with "
                quotedToken += token + " ";
            } while (token[token.size() - 1] != '"' and wordSeparator >> token);
            quotedToken.pop_back(); // removes the additional whitespace that gets added to the end because of the loop
            if (token[token.size() - 1] != '"') {
                std::cerr << "ERROR: unmatched quote in input.\n";
                return {};
            }
            tokens.push_back(quotedToken);
        }


        else { tokens.push_back(token); }
    }
    return tokens;
}


// tokenizes the input and deals with '&&' logic. will update in the future so it works properly
// finalWriteHandle is so we can retrieve the output of embedded commands like this 'cmd1 $(cmd2) input'
void inputHandler(const string& userInput, HANDLE& finalWriteHandle) {
    vector<string> tokens = tokenizeInput(userInput, true);
    if (tokens.empty()) { return; }
    vector<vector<string>> individualCommands = separateTokensByToken(tokens, "&&");
    try {
      for (vector<string> command : individualCommands) {
        pipeHandler(command, finalWriteHandle);
      }
    }
    catch (std::runtime_error& e) {
      cerr << "ARDO ERROR: An unexpected error occurred. " << e.what() << endl;
    }

}

