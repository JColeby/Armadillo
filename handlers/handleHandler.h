#pragma once
#include "../commonFunctions/handleIO.h"
#include <windows.h>


class HandleBase {
public:
    HandleBase() = default;

    HandleBase(HANDLE h, bool isPipeEnd)
        : handle(h), isPipe(isPipeEnd) {}

    // declaring that this function can't be copied (unless moved as seen below). This is so we don't accidentally close the handle twice
    HandleBase(const HandleBase&) = delete;
    HandleBase& operator=(const HandleBase&) = delete;


    // These next two sections of code dictate how the class is to behave when we pass it into std::move
    // std::move allows us to transfer ownership of the handle to a new object.

    // this is the move constructor, and is what runs when you use std::move to pass the class into a new function
    // This means that the handle will be closed at the end of that thread or function
    HandleBase(HandleBase&& other) noexcept {
        handle = other.handle;
        isPipe = other.isPipe;
        other.handle = nullptr;
        other.isPipe = false;
    }

    // this is the move operator, and is what runs when we want to use std::move to transfer the handle to an existing class
    HandleBase& operator=(HandleBase&& other) noexcept {
        if (this != &other) {
            cleanup();
            handle = other.handle;
            isPipe = other.isPipe;
            other.handle = nullptr;
            other.isPipe = false;
        }
        return *this;
    }


    virtual ~HandleBase() {
        cleanup();
    }

    // will throw an error if the caller doesn't save the result
    [[nodiscard]] HANDLE get() const { return handle; }

protected:
    HANDLE handle = nullptr;
    bool   isPipe = false;

    // ---- helpers ----

    [[nodiscard]] bool valid() const {
        return handle && handle != INVALID_HANDLE_VALUE;
    }

    // implemented by derived classes
    [[nodiscard]] virtual bool isStdHandle() const = 0;
    virtual void pipeCleanup() {}

private:
    void cleanup() {
        if (!valid()) return;
        if (isStdHandle()) return;

        if (isPipe) {
            pipeCleanup();
        }

        CloseHandle(handle);
        handle = nullptr;
    }
};



class ReadHandle final : public HandleBase {
public:
    ReadHandle() = default;

    explicit ReadHandle(HANDLE h, bool isPipeEnd = false)
        : HandleBase(h, isPipeEnd) {}

private:
    [[nodiscard]] bool isStdHandle() const override {
        return handle == GetStdHandle(STD_INPUT_HANDLE);
    }

    void pipeCleanup() override {
        HDL::drainPipe(handle);
    }
};



class WriteHandle final : public HandleBase {
public:
    WriteHandle() = default;

    explicit WriteHandle(HANDLE h, bool isPipeEnd = false)
        : HandleBase(h, isPipeEnd) {}

private:
    [[nodiscard]] bool isStdHandle() const override {
        return handle == GetStdHandle(STD_OUTPUT_HANDLE);
    }

    void pipeCleanup() override {
        FlushFileBuffers(handle);
        DisconnectNamedPipe(handle);
    }
};



// allows us to hold all the handles in a single location
struct StdHandles {
    ReadHandle*  read;
    WriteHandle* write;
    WriteHandle* error;

    StdHandles(ReadHandle* r, WriteHandle* w, WriteHandle* e)
        : read(r), write(w), error(e) {}
};
