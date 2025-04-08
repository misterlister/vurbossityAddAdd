#!/bin/bash

# Confirm that exactly one argument was passed
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path-to-file-with-or-without-extension>"
    exit 1
fi

# Get the current directory
CURRENT_DIR="$(pwd)"

# Extract the file's base name without the extension/filepath
INPUT_PATH="$1"
BASENAME="$(basename "$INPUT_PATH" .vurb)"

# Set up output directories
CPP_DIR="${CURRENT_DIR}/cppFiles"
EXE_DIR="${CURRENT_DIR}/executables"

# Make sure output directories exist
mkdir -p "$CPP_DIR"
mkdir -p "$EXE_DIR"

# Set output file paths
CPP_FILE="${CPP_DIR}/${BASENAME}.cpp"
EXE_FILE="${EXE_DIR}/${BASENAME}x"

# Convert VurbAddAdd to C++
./VaaToCpp < "${INPUT_PATH}" > "${CPP_FILE}"

# Compile the C++ file
g++ "${CPP_FILE}" -o "${EXE_FILE}"

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo "C++ file: ${CPP_FILE}"
    echo "Executable: ${EXE_FILE}"
else
    echo "Compilation failed."
    exit 1
fi
