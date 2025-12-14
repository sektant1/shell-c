#!/bin/bash

set -e # Exit early if any commands fail

# Get the script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Get the project name from the directory name
PROJECT_NAME="$(basename "$SCRIPT_DIR")"

# Default build type is debug
BUILD_TYPE="debug"
# Check for argument to set build type
if [ "$1" = "--release" ] || [ "$1" = "-r" ]; then
    BUILD_TYPE="release"
    shift # Remove the argument so it doesn't get passed to the binary
fi

# Ensure compile steps are run within the repository directory
cd "$SCRIPT_DIR"

# Create and build in the appropriate directory
BUILD_DIR="build/$BUILD_TYPE"
echo "Building $PROJECT_NAME in $BUILD_DIR..."

# Configure and build
cmake -B "$BUILD_DIR" -S . -DCMAKE_BUILD_TYPE="${BUILD_TYPE^}"  
cmake --build "$BUILD_DIR"

# Execute the binary (which should have the same name as the project)
EXECUTABLE="$BUILD_DIR/$PROJECT_NAME"
echo "Running: $EXECUTABLE $@"
exec "$EXECUTABLE" "$@"
