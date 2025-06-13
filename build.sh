#!/bin/bash

# Miracle Engine Build Script

echo "Building Miracle Engine..."

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build the project
echo "Compiling..."
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Run './build/miracle' to start the engine"
else
    echo "Build failed!"
    exit 1
fi
