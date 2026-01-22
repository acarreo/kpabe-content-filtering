#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Function to handle cleanup
cleanup() {
  if [ $? -eq 0 ]; then
    echo "Cleaning up..."
    cd .. && rm -rf $build_dir
  fi
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

# Check if dependencies are installed: RELIC and abe_lsss
if [ ! -d "/usr/local/include/relic" ] || [ ! -d "/usr/local/include/abe_lsss" ]; then
  echo "Dependencies not installed. Please install RELIC and abe_lsss."
  exit 1
fi

# Install google benchmark if not already installed
if [ ! -d "benchmark" ]; then
  ./google-benchmark.sh
fi

# Run the benchmark
PID=$$
build_dir="/tmp/build--bench--$PID"
bench_dir=$(pwd)

# Create build directory
mkdir $build_dir

# Run cmake and make
# run_cmake "$1"
cmake -B $build_dir -S $bench_dir

cd $build_dir
make -j

echo -e "\n.............................. Running benchmarks ..............................\n"
uname -a > benchmark.log

# Run benchmarks and log output
make run_benchmarks # &>> benchmark.log

# Check if benchmark.log exists before creating tar file
if [ ! -f benchmark.log ]; then
  echo "Benchmark log not found."
  exit 1
fi

# Create a tar file with the csv files and the benchmark log
tar -czf $PID--results.tar.gz benchmark--*.csv benchmark.log

# Move the tar file to the results directory
cd $bench_dir
mkdir -p ../results && mv $PID--results.tar.gz ../results/

# Clean up
make clean

echo -e "\n--- Benchmark completed successfully. Results are in the results directory. ---\n"
