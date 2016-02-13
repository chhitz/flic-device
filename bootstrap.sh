#!/bin/sh

git submodule init
git submodule update

pushd .
cd external/cppformat
cmake .
make cppformat
popd

