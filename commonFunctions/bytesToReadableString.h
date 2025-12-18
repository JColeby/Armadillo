#pragma once
#include <iomanip>
#include <sstream>
#include <string>

using std::string;

// takes in a double containing the size of data in bytes and converts it into a readable string
inline string bytesToReadableString(double size) {
  int unitsTracker = 1;
  while (size >= 1024) {
    unitsTracker += 1;
    size /= 1024;
  }
  string units;
  switch (unitsTracker) {
  case 1: units = "bytes"; break;
  case 2: units = "KB"; break;
  case 3: units = "MB"; break;
  case 4: units = "GB"; break;
  case 5: units = "TB"; break;
  default: // converting anything larger than a TB down to TB
    for (int i = 5 - unitsTracker; i > 0; i--) { size *= 1024; }
    units = "TB"; break;
  }

  std::stringstream output;
  output << std::fixed << std::setprecision(2) << size; // Format to 2 decimal places
  string numStr = output.str();
  // removes the decimal if it's a whole number
  if (numStr.find(".00") != string::npos) {
    numStr.erase(numStr.size() - 3);
  }

  return numStr + " " + units;
}