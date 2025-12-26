#pragma once
#include <windows.h>
#include <iostream>
#include <streambuf>


class redCerr : public std::streambuf {
public:
    explicit redCerr(std::streambuf* original_) {
        original = original_;
        hErr = GetStdHandle(STD_ERROR_HANDLE);
        GetConsoleScreenBufferInfo(hErr, &info);
    }

protected:
    // called when a single character is being passed to the buffer
    int overflow(int ch) override {
        if (ch == EOF) { return EOF; }
        set_red();
        int r = original->sputc(ch);
        restore();
        return r;
    }

    // called when multiple characters are being passed to the stream
    std::streamsize xsputn(const char* s, std::streamsize n) override {
        set_red();
        std::streamsize r = original->sputn(s, n);
        restore();
        return r;
    }

private:
    void set_red() {
        if (console_ok()) {
            GetConsoleScreenBufferInfo(hErr, &info);
            SetConsoleTextAttribute(hErr, FOREGROUND_RED | FOREGROUND_INTENSITY);
        }
    }

    void restore() {
        if (console_ok()) { SetConsoleTextAttribute(hErr, info.wAttributes); }
    }

    // checks to see if the handle is a valid console handle and not a pipe
    bool console_ok() const {
        DWORD mode;
        return hErr != INVALID_HANDLE_VALUE && GetConsoleMode(hErr, &mode);
    }

    std::streambuf* original;           // the actual buffer for cerr
    HANDLE hErr;                        // cerr handle
    CONSOLE_SCREEN_BUFFER_INFO info;    // saves the previous state of the console
};


inline void setCerrColorRed() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode = 0;
    GetConsoleMode(hOut, &consoleMode);
    if (consoleMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) {
        static redCerr redBuf(std::cerr.rdbuf());
        std::cerr.rdbuf(&redBuf);
    }
}

