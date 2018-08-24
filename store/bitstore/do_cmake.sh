#!/bin/bash

# git clone submodules
git submodule update --init --recursive

# env check
if test -e build; then
    echo "build dir already exists; rm -rf build"
    rm -rf build
fi

if test -e product; then
    echo "product dir already exists; rm -rf product"
    rm -rf product
fi

# cmake
mkdir build && cd build
cmake -DBOOST_J=$(nproc) $ARGS "$@" ..

# make
make clean
make -j 8