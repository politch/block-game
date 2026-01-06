#!/bin/sh

cd /cache || exit

mkdir -p build
cd build || exit
emcmake cmake -G Ninja /src/
ninja

mkdir -p /out
cp src/BlockGame.js /out/
cp src/BlockGame.wasm /out/
cp src/BlockGame.data /out/
cp /src/main.html /out/
cp /src/image.png /out/
