#!/bin/bash

export RELIC_TAG="0.6.0"
export CURVE="bls12-381"
export RELIC_INCLUDE="/usr/local/include/relic"
WORKDIR=$PWD

sudo apt-get update
sudo apt-get install -y cmake make g++ ca-certificates git libgmp-dev libmsgpack-dev flex libfl-dev pkg-config

# Upgrade gcc/g++ to 11 if necessary
gcc_version=$(gcc --version 2>&1 | grep -o '([0-9]+\.[0-9]+\.[0-9]+)' | head -1)
if [[ $(echo $gcc_version |cut -d'.' -f1) -le 11 ]]; then
    sudo apt-repository ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get install -y gcc-11 g++-11
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 20
    sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 20
fi

# Build RELIC
git clone https://github.com/relic-toolkit/relic /tmp/relic
cd /tmp/relic
git checkout -b compile-temp ${RELIC_TAG}
mkdir "target_${CURVE}"
cd "target_${CURVE}"
../preset/x64-pbc-${CURVE}.sh ..
make -j
sudo make install
make clean

# Build LSSS
cd $WORKDIR
# git submodule update --init --remote ../abe-lsss
cd ../abe-lsss
make
sudo make install
make clean
