@echo off
setlocal enabledelayedexpansion

echo Verifying CMake is installed...
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo CMake is not installed.
    echo Installation files for CMake can be found at cmake.org/download
    echo If you don't have a compiler, then you will also want to download g++ from msys2.org and follow the setup instructions there.
    pause
    exit /b 1
)


echo Verifying g++ is installed...
where g++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo g++ is not installed.
    echo Please download g++ from msys2.org and follow the setup instructions there.
    choice /C YN /M "Would you like to try installing Armadillo with a different compiler instead?"
    if !ERRORLEVEL! EQU 2 (
        echo WARNING: Armadillo has not been tested with other compilers! Support is not guaranteed!
        pause
    )
    else (
        echo Exiting...
        exit /b 1
    )
)



echo Building project...
echo.

REM Set Armadillo path
REM If you want to save ardo to a different location, you need to update both the ARMADILLO environment variable and the path in path.h
set "ARMADILLO=%LOCALAPPDATA%\Armadillo"

if not exist "%ARMADILLO%" mkdir "%ARMADILLO%"
if not exist "%ARMADILLO%\cmd" mkdir "%ARMADILLO%\cmd"
if not exist "%ARMADILLO%\resources" mkdir "%ARMADILLO%\resources"
if not exist "%ARMADILLO%\configurations" mkdir "%ARMADILLO%\configurations"
if not exist "%ARMADILLO%\build" mkdir "%ARMADILLO%\build"

cmake -S . -B "%ARMADILLO%\build" -G "MinGW Makefiles"
cmake --build "%ARMADILLO%\build" --config Debug


echo.
echo Copying resources, configurations, and executables to their correct locations...

robocopy "resources" "%ARMADILLO%\resources" /E /MIR > nul
robocopy "configurations" "%ARMADILLO%\configurations" /E /MIR > nul
robocopy "cmd\builtin\manuals" "%ARMADILLO%\cmd\builtinManuals" /E /MIR > nul
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

echo Build complete!
pause
