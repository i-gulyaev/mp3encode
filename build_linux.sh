#!/bin/bash


# build lame
pushd 3pp
pushd lame-3.99.5

./configure --enable-shared=no --enable-static=yes --disable-frontend --disable-decoder
make

popd
popd

pushd app
mkdir build
pushd build
cmake ..
make
popd
popd
