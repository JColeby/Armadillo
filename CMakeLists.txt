cmake_minimum_required(VERSION 3.10)
project(Armadillo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------
# Shared source files
# -----------------------
set(SHELL_SOURCES
        handlers/commandHandler.cpp
        handlers/embeddedHandler.cpp
        handlers/inputHandler.cpp
        handlers/pipeHandler.cpp
        TerminalFormatting.h
)

# -----------------------
# Main executable
# -----------------------
add_executable(Ardo
        armadillo.cpp
        ${SHELL_SOURCES}
)
add_executable(example cmd/custom/example/example.cpp)
