@echo off

REM Set Armadillo path
REM If you want to save ardo to a different location, you need to update both the ARMADILLO environment variable and the path in path.h
set "ARMADILLO=%LOCALAPPDATA%\Armadillo"

if not exist "%ARMADILLO%" mkdir "%ARMADILLO%"
if not exist "%ARMADILLO%\cmd" mkdir "%ARMADILLO%\cmd"
if not exist "%ARMADILLO%\resources" mkdir "%ARMADILLO%\resources"
if not exist "%ARMADILLO%\configurations" mkdir "%ARMADILLO%\configurations"

echo Copying resources and configurations
robocopy "resources" "%ARMADILLO%\resources" /E /MIR
robocopy "configurations" "%ARMADILLO%\configurations" /E /MIR
robocopy "cmd\builtin\manuals" "%ARMADILLO%\cmd\builtinManuals" /E /MIR


echo Building project

if not exist "build" mkdir build

cd build
cmake -G "MinGW Makefiles" ..
cmake --build . --config Debug
cd ..

copy "build\Ardo.exe" "%ARMADILLO%\Ardo.exe"


echo Copying files and executables to their correct locations

for /f "usebackq delims= eol=#" %%C in ("configurations\standardList.config") do (
    if not exist "%ARMADILLO%\cmd\standard\%%C" mkdir "%ARMADILLO%\cmd\standard\%%C"
    if exist "cmd\standard\%%C\manual.txt" copy "cmd\standard\%%C\manual.txt" "%ARMADILLO%\cmd\standard\%%C\manual.txt"
    if exist "build\%%C.exe" copy "build\%%C.exe" "%ARMADILLO%\cmd\standard\%%C\%%C.exe"
)

for /f "usebackq delims= eol=#" %%C in ("configurations\customList.config") do (
    if not exist "%ARMADILLO%\cmd\custom\%%C" mkdir "%ARMADILLO%\cmd\custom\%%C"
    if exist "cmd\custom\%%C\manual.txt" copy "cmd\custom\%%C\manual.txt" "%ARMADILLO%\cmd\custom\%%C\manual.txt"
    if exist "build\%%C.exe" copy "build\%%C.exe" "%ARMADILLO%\cmd\custom\%%C\%%C.exe"
)

echo Build complete.
pause
