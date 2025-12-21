#ifdef _MSC_VER
#pragma comment(lib, "Psapi.lib")
#endif
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>
#include <psapi.h>

#include "../../../TerminalFormatting.h"
#include "../../../commonFunctions/bytesToReadableString.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

using namespace VT;


struct Options {
  bool getArchitecture = false;
  bool getCpu = false;
  bool getMemory = false;
  bool getPerformance = false;
  bool getDrive = false;
};

const char* red = "";
const char* white = "";
const char* reset = "";



// I recommend setting flags and interpreting input like this.
bool validateSyntaxAndSetFlags(std::vector<std::string> &tokenizedInput, Options& opt) {

  for (size_t i = 1; i < tokenizedInput.size(); i++) { // looping through each parameter
    const std::string& param = tokenizedInput[i];

    // setting flags
    if (param[0] == '-') {
      for (int j = 1; j < param.size(); j++) {
        switch (param[j]) {
          // just set boolean values for the normal flags
          case 'a': opt.getArchitecture = true; break;
          case 'c': opt.getCpu = true; break;
          case 'd': opt.getDrive = true; break;
          case 'm': opt.getMemory = true; break;
          case 'p': opt.getPerformance = true; break;
          default: std::cerr << "SYNTAX ERROR: Unexpected flag: -" << param[j] << std::endl; return false;
        }
      }
    }

    // setting normal Items
    else {
      std::cerr << "SYNTAX ERROR: Too many arguments provided" << std::endl; return false;
    }
  }

  // if there were no flags, then we just set them all to true
  if (!opt.getArchitecture && !opt.getCpu && !opt.getDrive && !opt.getMemory && !opt.getPerformance) {
    opt.getArchitecture = true;
    opt.getCpu = true;
    opt.getDrive = true;
    opt.getMemory = true;
    opt.getPerformance = true;
  }
  return true;
}


string colorText(const string& message, std::string_view color) {
  std::stringstream formattedMessage;
  formattedMessage << color << message << reset;
  return formattedMessage.str();
}


  string getArchitectureInfo(SYSTEM_INFO system_info) {
    // deciphering the architecture
    switch (system_info.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64:
      return "  System Architecture: AMD x64 \n";
    case PROCESSOR_ARCHITECTURE_ARM:
      return "  System Architecture: ARM \n";
    case PROCESSOR_ARCHITECTURE_ARM64:
      return "  System Architecture: ARM x64 \n";
    case PROCESSOR_ARCHITECTURE_IA64:
      return "  System Architecture: IA-64 \n";
    case PROCESSOR_ARCHITECTURE_INTEL:
      return "  System Architecture: INTEL x86 \n";
    default:
      return "  System Architecture: UNKNOWN \n";
    }
  }



  string getCpuInfo(SYSTEM_INFO system_info) {
    std::stringstream outputBuffer;

    // because GetLogicalProcessorInformation returns an array of structures, we'll need to get the length of the buffer to
    // pass in. Fortunately GetLogicalProcessorInformation will do this for us if we pass in a nullptr.
    // It saves the required array size in bytes to DWORD len.
    DWORD len = 0;
    GetLogicalProcessorInformation(nullptr, &len);

    // We can then calculate the number of SYSTEM_LOGICAL_PROCESSOR_INFORMATION structs that will be returned,
    // which we use to allocate an array of the same size
    DWORD count = len / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
    vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(count);

    // now we can pass in our buffer and get the info we need!
    if (!GetLogicalProcessorInformation(buffer.data(), &len)) {
      return colorText("\nERROR: Failed to get core info \n", red);
    }

    int physicalCores = 0;
    for (const auto& info : buffer) {
      // RelationProcessorCore represents info about a core.
      // Since we are only interested in the number of cores, we don't do anything else with it.
      if (info.Relationship == RelationProcessorCore) { physicalCores++; }
    }

    int logicalProcessors = (int)system_info.dwNumberOfProcessors;
    int activeProcessors = 0;

    // because dwActiveProcessorMask is a bitmask, with each bit representing a processor, we have to
    // loop through each bit. Which is why we have this weird setup
    DWORD_PTR activeProcessorsMask = system_info.dwActiveProcessorMask;
    for (DWORD i = 0; i < sizeof(DWORD_PTR) * 8; i++) {
      if (activeProcessorsMask & ((DWORD_PTR)1 << i)) { activeProcessors += 1; }
    }

    outputBuffer << white << "\n====={ CPU Information }=====\n" << reset;
    outputBuffer << "  Number of Cores: " << std::to_string(physicalCores) << "\n";
    outputBuffer << "  Logical Processors: " << std::to_string(logicalProcessors) << "\n";
    outputBuffer << "  Active Processors: " << std::to_string(activeProcessors) << "\n";
    return outputBuffer.str();
  }



  string getMemoryInfo() {
    std::stringstream outputBuffer;

    MEMORYSTATUSEX memoryState = {};
    memoryState.dwLength = sizeof(memoryState);
    if (!GlobalMemoryStatusEx(&memoryState)) {
      return colorText("\nERROR: Failed to get memory status \n", red);
    }
    outputBuffer << white << "\n====={ Memory Information }=====\n" << reset;
    outputBuffer << "  Memory Usage: " << memoryState.dwMemoryLoad << "%\n";
    outputBuffer << "  Memory Installed: " << bytesToReadableString((double)memoryState.ullTotalPhys) << "\n";
    outputBuffer << "  Memory Available: " << bytesToReadableString((double)memoryState.ullAvailPhys) << "\n";
    return outputBuffer.str();
  }



  // similar to the other functions, where we query windows and output what was returned
  string getPerformanceInfo() {
    PERFORMANCE_INFORMATION performanceInfo;
    performanceInfo.cb = sizeof(PERFORMANCE_INFORMATION);

    if (!GetPerformanceInfo(&performanceInfo, performanceInfo.cb)) {
      return colorText("\nERROR: Failed to get performance info \n", red);
    }
    // a lot of the returned info is in pages, so we use this to convert it to bytes.
    auto pgSize = (double)performanceInfo.PageSize;

    std::stringstream outputBuffer;
    outputBuffer << white << "\n====={ Performance Information }=====\n" << reset;
    outputBuffer << "  Page Size: " << bytesToReadableString(pgSize) << "\n";
    outputBuffer << "  Committed Pages: " << performanceInfo.CommitTotal << " pages\n";
    outputBuffer << "  Current Page Limit: " << performanceInfo.CommitLimit << " pages\n";
    outputBuffer << "  Allocated Kernel Memory: " << bytesToReadableString((double)performanceInfo.KernelTotal * pgSize) << "\n";
    outputBuffer << "  Paged Kernel Memory: " << bytesToReadableString((double)performanceInfo.KernelPaged * pgSize) << "\n";
    outputBuffer << "  System Cache Memory: " << bytesToReadableString((double)performanceInfo.SystemCache * pgSize) << "\n";
    outputBuffer << "  Handle Count: " << performanceInfo.HandleCount << "\n";
    outputBuffer << "  Process Count: " << performanceInfo.ProcessCount << "\n";
    outputBuffer << "  Thread Count: " << performanceInfo.ThreadCount << "\n";

    return outputBuffer.str();
  }



  // returns physical drive info. If the physical drive doesn't exist, the function returns an empty string
  // because there isn't a set function that will retrieve this info for us, we have use device/driver queries
  string getPhysicalDriveInfo(int driveIndex) {
    std::stringstream outputBuffer;

    // Building the path to the physical drive
    std::wstring path = L"\\\\.\\PhysicalDrive" + std::to_wstring(driveIndex);

    // This function creates or opens a file or I/O device.
    // for more information about what this is doing, please visit https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
    HANDLE diskHandle = CreateFileW(path.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr, OPEN_EXISTING, 0, nullptr );

    // if the drive we are looking for does not exist
    if (diskHandle == INVALID_HANDLE_VALUE) { return ""; }

    // Because the physical drive is an I/O device, we have to call DeviceIoControl to get the information
    // Unfortunately this function is the biggest multi-tool of a function that I have ever seen, so we have to
    // specify what type of query we are making using this structure
    STORAGE_PROPERTY_QUERY driveQuery = {};
    driveQuery.PropertyId = StorageDeviceProperty;
    driveQuery.QueryType = PropertyStandardQuery;

    // Because of the nature of this function, what gets returned will vary depending on what query we make.
    // This means that we have to create a generic buffer to store the info from the query.
    // In order to get the correct size for our buffer, we first have to get the header information.
    // We do this by querying DeviceIoControl twice, where the first query will retrieve the response header, which we know the size of.
    STORAGE_DESCRIPTOR_HEADER driveQueryHeader = {};
    DWORD bytesReturned = 0;

    // DeviceIoControl is smart and will prevent itself from going beyond the buffer. Because of this, we can just
    // pass in the header structure, which will contains all the additional information that will be needed for the actual query
    if (!DeviceIoControl(
            diskHandle, // this is the handle to our drive that we got earlier
            IOCTL_STORAGE_QUERY_PROPERTY, // this tells the function what type of query we are performing (known as the control code)
            &driveQuery, // this is the input buffer, which we pass in our query. The query type is associated with the control code
            sizeof(driveQuery), // size of our query
            &driveQueryHeader, // this is the buffer where our output will be stored
            sizeof(driveQueryHeader), // size of the output buffer
            &bytesReturned, // the number of bytes that were filled in our output buffer
            nullptr)) // last parameter is for asynchronous calls. I didn't dive in too deep because it's not needed
    {
      return colorText("\nERROR: Failed to get physical drive info for \\\\.\\PhysicalDrive" + std::to_string(driveIndex) + "\n", RED);
    }

    DWORD requiredSize = driveQueryHeader.Size; // now we have the size of the full query, which we can use to make a proper buffer
    vector<BYTE> properBuffer(requiredSize); // note to self: buffer.data() will return a pointer to the start of the buffer

    // now we can call it with our proper buffer that will contain everything from the call.
    if (!DeviceIoControl(diskHandle, IOCTL_STORAGE_QUERY_PROPERTY, &driveQuery, sizeof(driveQuery),
      properBuffer.data(), requiredSize, &bytesReturned, nullptr))
    {
      return colorText("\nERROR: Failed to get physical drive info for \\\\.\\PhysicalDrive" + std::to_string(driveIndex) + "\n", RED);
    }

    // now we just have to redefine the buffer so the compiler knows it contains a STORAGE_DEVICE_DESCRIPTOR
    auto* driveDescriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(properBuffer.data());


    // now windows was silly, and decided that the function wouldn't actually include information like the
    // VendorId or ProductId in the structure. Instead, they included offsets to the actual information, which is stored
    // after the STORAGE_DEVICE_DESCRIPTOR object in the buffer (stupid, I know).
    outputBuffer << "  PhysicalDrive" << driveIndex << ": " << "\n";

    // extracting the Product ID, which contains the model of our drive
    string model;
    if (driveDescriptor->ProductIdOffset != 0) {
      model = reinterpret_cast<const char*>(properBuffer.data() + driveDescriptor->ProductIdOffset);
    } else { model = "Unknown Model"; }
    outputBuffer << "     Drive Model: " << model << "\n";

    // extracting the Product ID, which contains the model of our drive. My SSD drives dont have this for some reason
    string vendor;
    if (driveDescriptor->VendorIdOffset != 0) {
      vendor = reinterpret_cast<const char*>(properBuffer.data() + driveDescriptor->VendorIdOffset);
    } else { vendor = "Unknown Vendor"; }
    outputBuffer << "     Drive Vendor: " << vendor << "\n";

    // detecting if the drive is a USB
    if (driveDescriptor->RemovableMedia) { outputBuffer << "     Removable Media\n"; }
    else { outputBuffer << "     Installed Media\n"; }

    return outputBuffer.str();
  }



  string getDriveInfo() {
    std::stringstream outputBuffer;

    outputBuffer << white << "\n====={ Physical Drive Information }=====\n" << reset;
    // windows path to a physical drive will always look like \\.\PhysicalDrive<number>
    // each physical drive filepath is stored in sequence, so the first drive will be located at \\.\PhysicalDrive0, the next at \\.\PhysicalDrive1, and so on
    // Most computers can only store up to 32 drives, which is why our loop stops there.
    // view getPhysicalDriveInfo function for more information about how we obtain the drive information
    for (int i = 0; i < 32; i++) {
      string physicalDriveOutput = getPhysicalDriveInfo(i);
      if (physicalDriveOutput.empty()) { break; }
      outputBuffer << physicalDriveOutput;
    }


    outputBuffer << white << "\n====={ Drive Partition Information }=====\n" << reset;
    // looping through all the characters from A to Z and shifting the bitmask, so the char i will correspond to the correct drive
    // Querying the drives available.
    // GetLogicalDrives returns a bitmask that represents disk drives that are currently available
    // Bit position 0 (the least-significant bit) is drive A, bit position 1 is drive B, bit position 2 is drive C, and so on.
    DWORD driveBitmask = GetLogicalDrives();
    if (!driveBitmask) { return colorText("\nERROR: Failed to get drive info \n", red); }

    for (wchar_t i = 'A'; i <= 'Z'; i++) {
      if (driveBitmask & 1) {
        // setting up our buffers
        wchar_t drivePath[5] = { i, L':', L'\\', L'\\', L'\0' }; // because the function we call next needs to take in a pointer to a list of wide characters
        ULARGE_INTEGER freeBytesAvailable;
        ULARGE_INTEGER totalNumberOfBytes;
        ULARGE_INTEGER totalNumberOfFreeBytes;

        if (!GetDiskFreeSpaceExW(drivePath, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
          outputBuffer << red << "  ERROR: Failed to fetch info for Drive " << i << "\n" << reset; continue;
        }
        outputBuffer << "  " << (char)i << "-Drive: " << "\n";
        outputBuffer << "     Partition size: " << bytesToReadableString((double)totalNumberOfBytes.QuadPart) << "\n";
        outputBuffer << "     Free space: " << bytesToReadableString((double)totalNumberOfFreeBytes.QuadPart) << "\n";
        outputBuffer << "     Space available: " << bytesToReadableString((double)freeBytesAvailable.QuadPart) << "\n";
      }
      driveBitmask = driveBitmask >> 1;
    }

    return outputBuffer.str();
  }



int main(int argc, char* argv[]) {
    // standard setup
    Options opt;
    std::vector<std::string> tokenizedInput(argv, argv + argc); // convert it to a proper array
    if (!validateSyntaxAndSetFlags(tokenizedInput, opt)) { return EXIT_FAILURE; }

    if (verify()) {
      red = RED;
      white = WHITE;
      reset = RESET_TEXT;
    }

    // querying the computer architecture
    SYSTEM_INFO system_info;
    GetNativeSystemInfo(&system_info);
    std::stringstream outputBuffer;
    if (opt.getArchitecture) {
      outputBuffer << white << "\n====={ Architecture Information }=====\n" << reset;
      outputBuffer << getArchitectureInfo(system_info);
    }
    if (opt.getCpu) { outputBuffer << getCpuInfo(system_info); }
    if (opt.getMemory) { outputBuffer << getMemoryInfo(); }
    if (opt.getDrive) { outputBuffer << getDriveInfo(); }
    if (opt.getPerformance) { outputBuffer << getPerformanceInfo(); }
    outputBuffer << endl;
    cout << outputBuffer.str();
    return EXIT_SUCCESS;
}
