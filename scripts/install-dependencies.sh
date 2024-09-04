#!/bin/bash

ENABLE_COMPRESSION=${ENABLE_COMPRESSION:-OFF}
CURVE=${CURVE:-bls12-381}

echo "Installing dependencies..."

echo
echo "Installing RELIC library..."
cd ../abe-lsss/compile/
make CURVE=${CURVE}

echo
echo "Installing ABE-LSSS library..."
cd ../
mkdir -p build && cd build
cmake -DCOMPRESSION_ENABLED=${ENABLE_COMPRESSION} ..
make
sudo make install
