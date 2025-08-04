#!/bin/bash

set -eo pipefail

VENV_DIR=".venv"

echo "[0/6] Setting up Python virtual environment..."

if [ ! -d "$VENV_DIR" ]; then
    echo "Virtual environment not found. Creating one..."
    python3 -m venv "$VENV_DIR"
    echo "Virtual environment created."

    source "$VENV_DIR/bin/activate"

    echo "Upgrading pip and installing Conan..."

    pip install --upgrade pip
    pip install conan
else
    source "$VENV_DIR/bin/activate"
    echo "Virtual environment activated."
fi

brew install ninja

echo "[1/6] Detecting system profile..."
conan profile detect --force

echo "[2/6] Installing dependencies..."
mkdir -p build
conan install . --output-folder=build --build=missing --profile=conan_profile_mac.txt --deployer=direct_deploy

echo "[3/6] Entering build directory..."
cd build

echo "[4/6] Checking compiler and selecting CMake preset..."

BUILD_DIR=CMakeBuild

cmake -S .. -B $BUILD_DIR -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo "[5/6] Building the project..."

cmake --build $BUILD_DIR --config Release -- -j26

cd ..

cp build/$BUILD_DIR/compile_commands.json .

echo "[6/6] Build finished."

# rm -r .venv build CMakeUserPresets.json compile_commands.json .vscode/ .cache/