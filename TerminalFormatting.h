#pragma once
#include <string>

// Use these to add custom colors to your text

namespace VT {

    // --- Base ESC prefix ---
    constexpr const char* ESC = "\x1b[";

    // --- Reset / Styles ---
    constexpr const char* RESET_TEXT   = "\x1b[0m";
    constexpr const char* BOLD         = "\x1b[1m";
    constexpr const char* DIM          = "\x1b[2m";
    constexpr const char* ITALIC       = "\x1b[3m";
    constexpr const char* UNDERLINE    = "\x1b[4m";
    constexpr const char* INVERT       = "\x1b[7m";
    constexpr const char* HIDE         = "\x1b[8m";

    // --- Text colors (foreground) ---
    constexpr const char* YELLOW  = "\x1b[93m";
    constexpr const char* BLUE    = "\x1b[94m";
    constexpr const char* RED     = "\x1b[91m";
    constexpr const char* WHITE   = "\x1b[97m";
    constexpr const char* GREY    = "\x1b[37m";
    constexpr const char* BLACK   = "\x1b[30m";
    constexpr const char* GREEN   = "\x1b[32m";
    constexpr const char* MAGENTA = "\x1b[35m";
    constexpr const char* CYAN    = "\x1b[36m";
    constexpr const char* DEFAULT = "\x1b[39m";

    // --- Background colors ---
    constexpr const char* BLACK_BG   = "\x1b[40m";
    constexpr const char* RED_BG     = "\x1b[41m";
    constexpr const char* GREEN_BG   = "\x1b[42m";
    constexpr const char* YELLOW_BG  = "\x1b[43m";
    constexpr const char* BLUE_BG    = "\x1b[44m";
    constexpr const char* MAGENTA_BG = "\x1b[45m";
    constexpr const char* CYAN_BG    = "\x1b[46m";
    constexpr const char* WHITE_BG   = "\x1b[47m";
    constexpr const char* DEFAULT_BG = "\x1b[49m";

    // --- 256-color & RGB helpers ---
    inline std::string fgColor256(int n) {
        return "\x1b[38;5;" + std::to_string(n) + "m";
    }

    inline std::string bgColor256(int n) {
        return "\x1b[48;5;" + std::to_string(n) + "m";
    }

    inline std::string setTextColor(int r, int g, int b) {
        return "\x1b[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
    }

    inline std::string setBgColor(int r, int g, int b) {
        return "\x1b[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
    }

} // namespace VT