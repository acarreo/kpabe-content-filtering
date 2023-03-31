#!/bin/bash

tar -czvf lsss.tar.gz --exclude='*.o' --exclude='*.a' ../lsss

docker build -t presto-content-filtering-use-case:relic-0.6.0--lsss .

rm -f lsss.tar.gz
