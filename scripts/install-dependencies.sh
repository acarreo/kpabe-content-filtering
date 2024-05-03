#!/bin/bash

echo "Installing dependencies..."

echo
echo "Clone the ABE-LSSS..."
git clone https://github.com/acarreo/abe-lsss.git /tmp/abe-lsss

echo
echo "Installing RELIC library..."
cd /tmp/abe-lsss/compile/
make

echo
echo "Installing ABE-LSSS library..."
cd ../
mkdir -p build && cd build
cmake ..
make && make install
