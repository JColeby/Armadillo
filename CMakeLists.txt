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
        cmd/builtin/cd.cpp
        cmd/builtin/alias.cpp
        cmd/builtin/help.cpp
        cmd/builtin/cmd.cpp
        cmd/builtin/man.cpp
        handlers/redirectionHandler.cpp
)

# -----------------------
# Main executable
# -----------------------
add_executable(Ardo
        armadillo.cpp
        ${SHELL_SOURCES}
)

add_executable(example cmd/custom/example/example.cpp)
add_executable(ls cmd/standard/ls/ls.cpp)
add_executable(grep cmd/standard/grep/grep.cpp)
add_executable(edit cmd/standard/edit/edit.cpp)
