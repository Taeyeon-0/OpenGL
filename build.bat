@echo off
REM === Windows Build Script ===

echo --- Installing dependencies ---
conan install . --build=missing --output-folder=build --profile=conan_profile_windows.txt --deployer direct_deploy
if %errorlevel% neq 0 exit /b %errorlevel%

REM === Enter build directory ===
cd build

REM === Locate latest Visual Studio installation ===
for /f "usebackq tokens=*" %%i in (
    `"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`
) do (
    set "VS_PATH=%%i"
)

REM === Set up MSVC environment ===
call "%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat" x64

REM === Configure build variables ===
set BUILD_DIR=CMakeBuild

REM === Run CMake configuration ===
cmake ^
    -S .. -G "Ninja" ^
    -B %BUILD_DIR% ^
    -D CMAKE_TOOLCHAIN_FILE="build/conan_toolchain.cmake" ^
    -D CMAKE_POLICY_DEFAULT_CMP0091=NEW ^
    -D CMAKE_BUILD_TYPE=Release ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
    -DCMAKE_C_COMPILER=cl ^
    -DCMAKE_CXX_COMPILER=cl

if %errorlevel% neq 0 exit /b %errorlevel%

echo --- Building the project ---
cmake --build %BUILD_DIR% --config Release -- -j26
if %errorlevel% neq 0 exit /b %errorlevel%

REM === Return to root directory ===
cd ..

REM === Copy compile_commands.json to root ===
copy "build\%BUILD_DIR%\compile_commands.json" .

echo Build finished.