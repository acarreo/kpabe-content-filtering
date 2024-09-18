#!/bin/bash

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

# Run cmake and make, exit if any command fails
if ! cmake ..; then
  echo "CMake configuration failed."
  cd .. && rm -rf $build_dir
  exit 1
fi

if ! make -j; then
  echo "Make failed."
  cd .. && rm -rf $build_dir
  exit 1
fi

# Run benchmarks and log output
echo -e "\n\n.............................. Running benchmarks .............................\n"
if ! make run_benchmarks &> benchmark.log; then
  echo "Running benchmarks failed."
  cd .. && rm -rf $build_dir
  exit 1
fi

# Check if benchmark.log exists before creating tar file
if [ ! -f benchmark.log ]; then
  echo "Benchmark log not found."
  cd .. && rm -rf $build_dir
  exit 1
fi

# Create a tar file with the csv files and the benchmark log
tar -czf $PID--results.tar.gz benchmark--*.csv benchmark.log

# Move the tar file to the results directory
mkdir -p ../results && mv $PID--results.tar.gz ../results/

# Clean up
make clean
cd .. && rm -rf $build_dir

echo -e "\n--- Benchmark completed successfully. Results are in the results directory. ---\n"
