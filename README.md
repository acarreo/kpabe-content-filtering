# KP-ABE scheme for Content Filtering Use Case
This project implement KP-ABE using DPVS (Dual Pairing Vector Spaces) scheme for the _Content Filtering Use Case_.

You need [Docker](https://docs.docker.com/engine/install/) to run this project.

## Clone project
```bash
git clone "git@gitlab.com:adam92oumar/kpabe-content-filtering.git"
```

## Build Dcoker image
Install all dependencies, including [RELIC](https://github.com/relic-toolkit/relic) Library.
```bash
cd scripts && ./build_relic_lsss.sh
```

## Run test bed
```bash
./benchmarks.sh
```