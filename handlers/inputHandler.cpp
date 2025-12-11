#include "headers/inputHandler.h"
#include "../TerminalFormatting.h"
#include "../commonFunctions/listToString.h"





vector<string> tokenizeInput(const string& inputString, bool removeQuotes, bool dontTokenizeEmbedded) {
    std::istringstream wordSeparator(inputString);
    vector<string> tokens;
    string token;
    // separating characters by whitespace
    while (wordSeparator >> token) {


        if (token[0] == '"') { // logic for quoted input. quotes will be automatically removed
            string quotedToken;

            if (removeQuotes) { token = token.erase(0, 1); }
            do { // gets next token until we hit a word that ends with " or until the input ends
                quotedToken += token + " ";
            } while (token[token.size() - 1] != '"' and wordSeparator >> token);
            quotedToken.erase(quotedToken.size() - 1); // for some reason there is always an addition whitespace that gets thrown onto the end

            if (removeQuotes) { // removed trailing "
                if (quotedToken[quotedToken.size() - 1] == '"') { quotedToken.erase(quotedToken.size() - 1); }
            }
            tokens.push_back(quotedToken);
        }


        else if (dontTokenizeEmbedded and token.size() >= 2 and token[0] == '$' and token[1] == '(') { // logic for quoted input. quotes will be automatically removed

            int parenthesesCount = 0;
            string embeddedCommand;

            do { // gets next token until all parentheses are closed
                int opi = 0; // opening parentheses index
                while (opi < token.size() - 1 and token[opi] == '$' and token[opi+1] == '(') {parenthesesCount++; opi += 2; }

                int cpi = token.size() - 1; // closing parentheses index
                while (cpi >= 0 and token[cpi] == ')') { parenthesesCount--; cpi--; }

                embeddedCommand += token + " ";
            } while (parenthesesCount != 0 and wordSeparator >> token);

            embeddedCommand.erase(embeddedCommand.size() - 1); // removes the additional whitespace that gets added to the end because of the loop
            tokens.push_back(embeddedCommand);
        }


        else { tokens.push_back(token); }

    }
    return tokens;
}


void inputHandler(const string& userInput) {
    vector<string> tokens = tokenizeInput(userInput, true, true);
    string tokenizedString = listToString(tokens);
    cout << tokenizedString << endl;
}