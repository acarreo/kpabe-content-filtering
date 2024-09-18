#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Function to handle cleanup
cleanup() {
  echo "Cleaning up..."
  # cd .. && rm -rf $build_dir
}

# Trap to ensure cleanup is performed on exit
trap cleanup EXIT

# Function to run CMake configuration
run_cmake() {
  local compression_flag=$1
  if [ "$compression_flag" == "OFF" ]; then
    cmake .. -DENABLE_COMPRESSION=OFF
  else
    cmake ..
  fi
}

# Check if dependencies are installed: RELIC and lsss_abe
if [ ! -d "/usr/local/include/relic" ] || [ ! -d "/usr/local/include/lsss_abe" ]; then
  echo "Dependencies not installed. Please install RELIC and lsss_abe."
  exit 1
fi

# Install google benchmark if not already installed
if [ ! -d "benchmark" ]; then
  ./google-benchmark.sh
fi

# Run the benchmark
PID=$$
build_dir="$PID--build"

# Create build directory and navigate into it
mkdir $build_dir && cd $build_dir

# Run cmake and make
run_cmake "$1"

make -j

echo -e "\n.............................. Running benchmarks ..............................\n"
uname -a > benchmark.log

# Run benchmarks and log output
make run_benchmarks &>> benchmark.log

# Check if benchmark.log exists before creating tar file
if [ ! -f benchmark.log ]; then
  echo "Benchmark log not found."
  exit 1
fi

# Create a tar file with the csv files and the benchmark log
tar -czf $PID--results.tar.gz benchmark--*.csv benchmark.log

# Move the tar file to the results directory
mkdir -p ../results && mv $PID--results.tar.gz ../results/

# Clean up
make clean

echo -e "\n--- Benchmark completed successfully. Results are in the results directory. ---\n"
