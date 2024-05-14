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

   # Clone ABE-LSSS submodule
   git submodule update --init --remote abe-lsss
   ```

2. **Install Dependencies:** Two ways to install dependencies __locally__ (for the host machine) or in a [Docker](https://docs.docker.com/engine/install/) environment, follow one of the following instructions:
   ```bash
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
      ```

2. **Build in a Docker Environment**
   ```bash
   cd .. # go back to the root directory
   docker run -ti --rm -v $PWD:/content-filtering presto-content-filtering-use-case:relic-0.6.0--lsss-0.1.1
   mkdir -p /content-filtering/target
   cd /content-filtering/target
   cmake ..
   make
   ```

- Run the `./kpabe_out` executable to perform the KP-ABE encryption and decryption.
- Run the `./new_examples` executable to see an example of how to use the KP-ABE scheme.


### Troubleshooting

   If need, add the library path `/usr/local/lib` to the environment variable or add it to the ~/.bashrc file and run `source ~/.bashrc`.

   ```bash
   echo 'export LD_LIBRARY_PATH=/usr/local/lib' >> ~/.bashrc
   source ~/.bashrc
   ```
