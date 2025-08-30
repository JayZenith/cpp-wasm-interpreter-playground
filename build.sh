#!/bin/bash

# Set up Emscripten environment
source /home/jay-zenith/Desktop/emsdk/emsdk_env.sh

# Build script to compile C++ interpreter to WebAssembly
# Make sure you have Emscripten installed first:
# git clone https://github.com/emscripten-core/emsdk.git
# cd emsdk
# ./emsdk install latest
# ./emsdk activate latest
# source ./emsdk_env.sh

echo "Building interpreter.wasm..."

# emcc interpreter.cpp \
#   -o interpreter.js \
#   -s WASM=1 \
#   -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
#   -s ALLOW_MEMORY_GROWTH=1 \
#   -s MODULARIZE=1 \
#   -s EXPORT_NAME="createInterpreterModule" \
#   --bind \
#   -O2 \
#   --std=c++14

em++ interpreter.cpp -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME="createInterpreterModule" -O3 -o interpreter.js -s ALLOW_MEMORY_GROWTH=1 --bind


echo "Build complete! Files generated:"
echo "- interpreter.js"
echo "- interpreter.wasm"
echo ""
echo "Copy both files to your React app's public/ directory."