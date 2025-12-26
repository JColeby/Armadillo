@echo off
setlocal enabledelayedexpansion

echo Copying resources and configurations to their correct locations...

REM Set Armadillo path
REM If you want to save ardo to a different location, you need to update both the ARMADILLO environment variable and the path in path.h
set "ARMADILLO=%LOCALAPPDATA%\Armadillo"

if not exist "%ARMADILLO%" mkdir "%ARMADILLO%"
if not exist "%ARMADILLO%\cmd" mkdir "%ARMADILLO%\cmd"
if not exist "%ARMADILLO%\resources" mkdir "%ARMADILLO%\resources"
if not exist "%ARMADILLO%\configurations" mkdir "%ARMADILLO%\configurations"

robocopy "resources" "%ARMADILLO%\resources" /E /MIR > nul
robocopy "configurations" "%ARMADILLO%\configurations" /E /MIR > nul
robocopy "cmd\builtin\manuals" "%ARMADILLO%\cmd\builtinManuals" /E /MIR > nul



echo Verifying g++ is installed...
where g++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo g++ is not installed.
    echo Please download g++ from msys2.org and follow the setup instructions there.
    echo It may be possible to compile using a different compiler using CMake.
    choice /C YN /M "Would you like to try installing Armadillo with a different compiler instead?"
    if !ERRORLEVEL! EQU 2 (
        echo WARNING: Armadillo has not been tested with other compilers! Support is not guaranteed!
        pause
        echo Verifying CMake is installed...
        goto :CMakeBuild
    )
    echo Exiting...
    exit /b 1
)

where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    goto :noCMakeBuild
)
echo CMake detected. Compiling Using CMake





:CMakeBuild

where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo CMake is not installed.
    echo Installation files for CMake can be found at cmake.org/download
    pause
    exit /b 1
)

echo Building project...

if not exist "%ARMADILLO%\build" mkdir "%ARMADILLO%\build"

cmake -S . -B "%ARMADILLO%\build" -G "MinGW Makefiles"
cmake --build "%ARMADILLO%\build" --config Debug

echo Moving executables...

copy "%ARMADILLO%\build\Ardo.exe" "%ARMADILLO%\Ardo.exe" > nul
copy "%ARMADILLO%\Ardo.exe" "Ardo.exe" > nul

for /d %%C in ("cmd\standard\*") do (
    if not exist "%ARMADILLO%\cmd\standard\%%~nxC" mkdir "%ARMADILLO%\cmd\standard\%%~nxC"
    if exist "cmd\standard\%%~nxC\manual.txt" copy "cmd\standard\%%~nxC\manual.txt" "%ARMADILLO%\cmd\standard\%%~nxC\manual.txt" > nul
    if exist "%ARMADILLO%\build\%%~nxC.exe" copy "%ARMADILLO%\build\%%~nxC.exe" "%ARMADILLO%\cmd\standard\%%~nxC\%%~nxC.exe" > nul
)

for /d %%C in ("cmd\custom\*") do (
    if not exist "%ARMADILLO%\cmd\custom\%%~nxC" mkdir "%ARMADILLO%\cmd\custom\%%~nxC"
    if exist "cmd\custom\%%~nxC\manual.txt" copy "cmd\custom\%%~nxC\manual.txt" "%ARMADILLO%\cmd\custom\%%~nxC\manual.txt" > nul
    if exist "%ARMADILLO%\build\%%~nxC.exe" copy "%ARMADILLO%\build\%%~nxC.exe" "%ARMADILLO%\cmd\custom\%%~nxC\%%~nxC.exe" > nul
)

goto :endOfProgram





:noCMakeBuild

echo Compiling Main Executable... (this may take a moment)
g++ -std=c++17 -g armadillo.cpp handlers\*.cpp cmd\builtin\*.cpp  -Iinclude -o "%ARMADILLO%\Ardo.exe"
copy "%ARMADILLO%\Ardo.exe" "Ardo.exe" > nul

for /d %%C in ("cmd\standard\*") do (
    echo Compiling Standard Command %%~nxC...
    if not exist "%ARMADILLO%\cmd\standard\%%~nxC" mkdir "%ARMADILLO%\cmd\standard\%%~nxC"
    if exist "cmd\standard\%%~nxC\manual.txt" copy "cmd\standard\%%~nxC\manual.txt" "%ARMADILLO%\cmd\standard\%%~nxC\manual.txt" > nul
    g++ -std=c++17 -g "cmd\standard\%%~nxC\*.cpp" -o "%ARMADILLO%\cmd\standard\%%~nxC\%%~nxC.exe"
)

for /d %%C in ("cmd\custom\*") do (
    echo Compiling Custom Command %%~nxC...
    if not exist "%ARMADILLO%\cmd\custom\%%~nxC" mkdir "%ARMADILLO%\cmd\custom\%%~nxC"
    if exist "cmd\custom\%%~nxC\manual.txt" copy "cmd\custom\%%~nxC\manual.txt" "%ARMADILLO%\cmd\custom\%%~nxC\manual.txt" > nul
    g++ -std=c++17 -g "cmd\custom\%%~nxC\*.cpp" -o "%ARMADILLO%\cmd\custom\%%~nxC\%%~nxC.exe"
)




:endOfProgram
echo Build complete!
pause