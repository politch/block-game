#!/bin/sh

set -eu

mkdir -p /cache
cd /cache || exit

export EM_CACHE="/cache"

mkdir -p build
mkdir -p /out
cd build || exit
emcmake cmake -G Ninja /src/ >/out/cmake.out 2>&1
ninja >/out/ninja.out 2>&1

mkdir -p /out
cp src/BlockGame.js /out/
cp src/BlockGame.wasm /out/
cp src/BlockGame.data /out/
cp /src/main.html /out/
cp /src/image.png /out/
