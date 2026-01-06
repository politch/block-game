#!/bin/sh

mkdir -p build
cd build || exit
emcmake cmake -G Ninja ..
ninja

mkdir -p /out
cp src/BlockGame.js /out/
cp src/BlockGame.wasm /out/
cp src/BlockGame.data /out/
cp ../main.html /out/
cp ../image.png /out/
