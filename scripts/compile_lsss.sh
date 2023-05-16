#!/bin/bash

tar -xzvf lsss.tar.gz
for incl_relic in /usr/include/relic_*; do
    export RELIC_INCLUDE=$incl_relic
    make -C lsss
    mv lsss/liblsss.a /usr/lib/liblsss_${incl_relic:19}.a
    make -C lsss clean
done

