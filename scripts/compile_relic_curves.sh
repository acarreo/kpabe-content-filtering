#!/bin/bash

list_curves=(
    "bls12-381"
    "bls12-446"
    "bn254"
    "bn382"
    "bn446"
)

if [ -d "/root/relic" ]; then
    for curve in ${list_curves[@]}; do
        mkdir "/root/relic/target_${curve}" && cd "/root/relic/target_${curve}"
        ../preset/x64-pbc-${curve}.sh .. && make -j && make install
        rm -rf "/root/relic/target_${curve}"
        mv /usr/local/include/relic /usr/include/relic_${curve}
        mv /usr/local/lib/librelic_s.a /usr/lib/librelic_s.a_${curve}
    done
else
    echo "[ ERRORS ] The directory '/root/relic' does not exist !"
    exit 1
fi
