cmake_minimum_required(VERSION 3.10)
project(Armadillo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------
# Main executable
# -----------------------
add_executable(Ardo
        armadillo.cpp
        handlers/commandHandler.cpp
        handlers/embeddedHandler.cpp
        handlers/inputHandler.cpp
        handlers/pipeHandler.cpp
        cmd/builtin/cd.cpp
        cmd/builtin/alias.cpp
        cmd/builtin/help.cpp
        cmd/builtin/cmd.cpp
        cmd/builtin/man.cpp
        handlers/redirectionHandler.cpp
        cmd/builtin/echo.cpp
        handlers/handleHandler.h
)


# -----------------------
# command executables
# -----------------------
add_executable(example cmd/custom/example/example.cpp)
add_executable(ls cmd/standard/ls/ls.cpp)
add_executable(grep cmd/standard/grep/grep.cpp)
add_executable(edit cmd/standard/edit/edit.cpp)
add_executable(pwd cmd/standard/pwd/pwd.cpp)
add_executable(cat cmd/standard/cat/cat.cpp)
add_executable(ps cmd/standard/ps/ps.cpp)
target_link_libraries(ps PRIVATE Psapi)
add_executable(sys cmd/standard/sys/sys.cpp)
target_link_libraries(sys PRIVATE Psapi)
add_executable(date cmd/standard/date/date.cpp)
add_executable(cp cmd/standard/cp/cp.cpp)
add_executable(mkdir cmd/standard/mkdir/mkdir.cpp)
add_executable(rm cmd/standard/rm/rm.cpp)
add_executable(touch cmd/standard/touch/touch.cpp)
