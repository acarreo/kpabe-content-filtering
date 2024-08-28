#!/bin/bash

echo "Installing dependencies..."

echo
echo "Installing RELIC library..."
cd ../abe-lsss/compile/
make CURVE=bls12-381

echo
echo "Installing ABE-LSSS library..."
cd ../
mkdir -p build && cd build
cmake -DCOMPRESSION_ENABLED=ON ..
make
sudo make install
