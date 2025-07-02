#!/bin/bash

# Couleurs
BLUE='\033[1;34m'
RED='\033[1;31m'
NC='\033[0m' # No Color

# Color functions 
info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

error() {
    echo -e "${RED}[ERREUR]${NC} $1" >&2
    exit 1
}

# Cleaning
info "Removing existing files 'build/' et 'bin/'..."
rm -rf build || error "Error in remouving file build/"
rm -rf bin || error "Error in remouving file bin/"

# CMake Configuration
info "Project configuration with CMake..."
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake || error "Error in CMake configuration"

# Compilation
info "Compilation ..."
cmake --build build || error "Compilation failed"

# Go to the bin file 
cd bin || error "Error in changing directory"

# Conversion .elf -> .bin
info "Converting .elf to .bin..."
arm-none-eabi-objcopy -O binary stm32_rt.elf stm32_rt.bin || error "Error in conversion"

# Flashing the card 
TARGET_PATH="/media/$USER/NOD_H743ZI2"
if [ -d "$TARGET_PATH" ]; then
    info "Copying bin file to ($TARGET_PATH)..."
    cp "stm32_rt.bin" "$TARGET_PATH" || error "Error in copying file"
    info "Firmware uploaded successfully"
else
    error "Error in finding $TARGET_PATH"
fi
