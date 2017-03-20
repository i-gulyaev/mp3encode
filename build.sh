#!/bin/bash


# build lame
pushd lame-3.99.5

./configure ----enable-shared=no --enable-static=yes --disable-frontend --disable-decoder
make

popd

pushd mp3encode
mkdir build
pushd build
cmake ..
make
popd
popd
