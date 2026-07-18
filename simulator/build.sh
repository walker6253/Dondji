#!/bin/bash
# Dondji Simulator - Quick build script
set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

echo "=== Dondji Simulator Build ==="

# Ensure SDL2 is available
if ! pkg-config --exists sdl2 2>/dev/null && ! brew list sdl2 &>/dev/null; then
    echo "SDL2 not found. Installing via brew..."
    brew install sdl2
fi

# Ensure cmake is available
export PATH="/opt/homebrew/bin:$PATH"

# Configure and build
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(sysctl -n hw.ncpu)

echo ""
echo "=== Build complete ==="
echo "Run: $BUILD_DIR/dondji_sim"
echo ""
echo "Keyboard mapping:"
echo "  0-9      Number keys"
echo "  M/Enter  Menu"
echo "  Up/Down  Navigation"
echo "  Esc      Exit/Back"
echo "  *        Star key"
echo "  F        Function key"
echo "  P        PTT"
echo "  +/-      Zoom in/out"
echo "  Q        Quit simulator"
