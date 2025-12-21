#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

struct Options {
  bool recursive = false;
  std::string target;
};


// I recommend setting flags and interpreting input like this.
bool validateSyntaxAndSetFlags(std::vector<std::string> &tokenizedInput, Options& opt) {
  if (tokenizedInput.empty()) { std::cerr << "SYNTAX ERROR: too few parameters" << std::endl; return false; }  // check that you don't have too few arguments
  int nonFlagCount = 0;
  for (size_t i = 1; i < tokenizedInput.size(); i++) { // looping through each parameter
    const std::string& param = tokenizedInput[i];

    // setting flags
    if (param[0] == '-') {
      for (int j = 1; j < param.size(); j++) {
        switch (param[j]) {
          case 'a': opt.recursive = true; break;
          default: std::cerr << "SYNTAX ERROR: Unexpected flag: -" << param[j] << std::endl; return false;
        }
      }
    }

    // setting normal Items
    else {
      switch (nonFlagCount) {
        case 0: opt.target = param; break;
        default: std::cerr << "SYNTAX ERROR: Too many arguments provided" << std::endl; return false;
      }
      nonFlagCount++;
    }
  }
  return true;
}



int main(int argc, char* argv[]) {
    // standard setup
    Options opt;
    std::vector<std::string> tokenizedInput(argv, argv + argc); // convert it to a proper array
    if (!validateSyntaxAndSetFlags(tokenizedInput, opt)) { return EXIT_FAILURE; }

    fs::path path(opt.target);

    if (!fs::exists(path)) {
        std::cerr << "ERROR: no such file or directory: " << opt.target << "\n";
        return EXIT_FAILURE;
    }

    if (fs::is_directory(path) && !opt.recursive) {
        std::cerr << "ERROR: cannot remove directory '" << opt.target << "'\n";
        return EXIT_FAILURE;
    }

    try {
        if (opt.recursive) {
            fs::remove_all(path);
        } else {
            if (!fs::remove(path)) {
                std::cerr << "ERROR: failed to remove file or directory\n";
                return EXIT_FAILURE;
            }
        }
    }

    catch (...) {
        std::cerr << "ERROR: failed to remove file or directory\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
