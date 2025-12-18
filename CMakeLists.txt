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
        cmd/builtin/echo.cpp
)

# -----------------------
# Main executable
# -----------------------
add_executable(Ardo
        armadillo.cpp
        ${SHELL_SOURCES}
)


if (WIN32)
    target_link_libraries(Ardo PRIVATE Psapi)
endif()

add_executable(example cmd/custom/example/example.cpp)
add_executable(ls cmd/standard/ls/ls.cpp)
add_executable(grep cmd/standard/grep/grep.cpp)
add_executable(edit cmd/standard/edit/edit.cpp)
add_executable(pwd cmd/standard/pwd/pwd.cpp)
add_executable(cat cmd/standard/cat/cat.cpp)
add_executable(ps cmd/standard/ps/ps.cpp)
add_executable(sys cmd/standard/sys/sys.cpp)
