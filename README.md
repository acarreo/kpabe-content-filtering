# KP-ABE Scheme for Content Filtering Use Case

This project implements the KP-ABE (Key-Policy Attribute-Based Encryption) using the DPVS (Dual Pairing Vector Spaces) scheme for the Content Filtering Use Case.

## Prerequisites
The project utilizes the [RELIC](https://github.com/relic-toolkit/relic)
library for cryptographic operations, the GMP library for arbitrary
precision arithmetic, and the ABE-LSSS library, which is a part of the
[OpenABE](https://github.com/zeutro/openabe) project.

## Installation

To install and run this project, follow these steps:

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/acarreo/kpabe-content-filtering.git
   cd kpabe-content-filtering
   ```

2. **Install Dependencies:** Two ways to install dependencies __locally__ (for the host machine) or in a [Docker](https://docs.docker.com/engine/install/) environment, follow one of the following instructions:
   ```bash
   # Clone ABE-LSSS submodule - only need if you want to compile the project on your host machine
   git submodule update --init --remote abe-lsss

   cd scripts
   # Install dependencies on the host machine
   make
   # OR Install dependencies in a Docker environment
   make docker
   ```

## Build and Run the Project
 Go to the `kpabe-content-filtering` directory and follow one of the following instructions to build and run the project:

1. **Build on the Host Machine**
      ```bash
      mkdir build && cd build
      cmake ..
      make

      # If you want to install the project as library, you need root privelege
      sudo make install
      ```

2. **Build in a Docker Environment**
   ```bash
   cd .. # go back to the root directory
   docker run -ti --rm -v $PWD:/content-filtering cryptolib/abe-lsss:relic-0.6.0--bls12-381--v0.1.0
   mkdir -p /tmp/build-kpabe
   cd /tmp/build-kpabe
   cmake /content-filtering
   make

   # If you want to install the project as library
   make install
   ```

- Run the `./kpabe_test` executable to perform a simple encryption and decryption.
- Run the `./kpabe_bench` executable to perform a simplist benchmark
- Run the `./kpabe_example` executable to see an example of how to use the KP-ABE scheme.


### Troubleshooting

   If need, add the library path `/usr/local/lib` to the environment variable or add it to the ~/.bashrc file and run `source ~/.bashrc`.

   ```bash
   echo 'export LD_LIBRARY_PATH=/usr/local/lib' >> ~/.bashrc
   source ~/.bashrc
   ```
