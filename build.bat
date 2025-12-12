@echo off

REM Set Armadillo path.
REM If you want to save ardo to a different path, you need to update both the ARMADILLO environment variable and the path in path.h
set "ARMADILLO=%LOCALAPPDATA%\Armadillo"


REM Create folders if they don't exist
if not exist "%ARMADILLO%" mkdir "%ARMADILLO%"
if not exist "%ARMADILLO%\cmd" mkdir "%ARMADILLO%\cmd"
if not exist "%ARMADILLO%\resources" mkdir "%ARMADILLO%\resources"


echo Copying Resources
robocopy "resources" "%ARMADILLO%\resources" /E /MIR


echo Compiling Main Executable
g++ -std=c++17 -g armadillo.cpp Handlers\*.cpp -Iinclude -o "%ARMADILLO%\Ardo.exe"
copy "%ARMADILLO%\Ardo.exe" "Ardo.exe"

for /f "usebackq delims= eol=#" %%C in ("cmd\standardList.config") do (

    echo Compiling Standard Command %%C
    if not exist "%ARMADILLO%\cmd\standard\%%C" mkdir "%ARMADILLO%\cmd\standard\%%C"

    REM Copy manual
    if exist "cmd\standard\%%C\manual.txt" copy "cmd\standard\%%C\manual.txt" "%ARMADILLO%\cmd\standard\%%C\manual.txt"

    REM Compile module
    g++ -std=c++17 -g cmd\standard\%%C\*.cpp -o "%ARMADILLO%\cmd\standard\%%C\%%C.exe"
)


for /f "usebackq delims= eol=#" %%C in ("cmd\customList.config") do (

    echo Compiling Custom Command %%C
    if not exist "%ARMADILLO%\cmd\custom\%%C" mkdir "%ARMADILLO%\cmd\custom\%%C"

    REM Copy manual if it exists
    if exist "cmd\custom\%%C\manual.txt" copy "cmd\custom\%%C\manual.txt" "%ARMADILLO%\cmd\custom\%%C\manual.txt"

    REM Compile module
    g++ -std=c++17 -g cmd\custom\%%C\*.cpp -o "%ARMADILLO%\cmd\custom\%%C\%%C.exe"
)