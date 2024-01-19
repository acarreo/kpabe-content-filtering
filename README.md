# KP-ABE Scheme for Content Filtering Use Case

This project implements the KP-ABE (Key-Policy Attribute-Based Encryption) using the DPVS (Dual Pairing Vector Spaces) scheme for the Content Filtering Use Case.

## Prerequisites
The project utilizes the [RELIC](https://github.com/relic-toolkit/relic) library
for cryptographic operations, the GMP library for arbitrary precision
arithmetic, and the LSSS (Linear Secret Sharing Schemes) library, which is
a part of the [OpenABE](https://github.com/zeutro/openabe) project.

## Installation

To install and run this project, follow these steps:

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/acarreo/kpabe-content-filtering.git
   cd kpabe-content-filtering
   mkdir build
   cd build
   cmake ..
   ```

2. **Install Dependencies*Two ways to install dependencies __locally__ (for the host machine) or in a [Docker](https://docs.docker.com/engine/install/) environment, follow one of the following instructions:
   ```bash
   make install_dependencies_local
   make install_dependencies_docker
   ```

## Build and Run the Project

1. **Build on the Host Machine**
      ```bash
      make
      make example
      ```

2. **Build in a Docker Environment**
   ```bash
   cd .. # go back to the root directory
   docker run -ti --rm -v $PWD:/content-filtering presto-content-filtering-use-case:relic-0.6.0--lsss-0.1.1
   mkdir -p /content-filtering/target
   cd /content-filtering/target
   cmake ..
   make
   make example
   ```

- Run the `./kpabe_out` executable to perform the KP-ABE encryption and decryption.
- Run the `./example_out` executable to see an example of how to use the KP-ABE scheme.
