/**
 * @file new_bench.cpp
 * @author Adam Oumar Abdel-rahman
 * @brief 
 * @date 2024-03-27
 *
 */


#include <iostream>
#include <fstream>
#include <chrono>

#include "new_bench.hpp"

using namespace std;
using namespace std::chrono;

bn_t Fq;

int main(int argc, char **argv)
{
  if (!init_libraries()) return 2;

  bench_serialization_params();
  // bench_serialization_dec_key();
  // bench_encryption();
  // bench_decryption();


  clean_libraries();
  return 0;
}

void bench_serialization_params(int rounds)
{
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    std::cerr << "Error: Could not setup keys" << std::endl;
    return;
  }

  KPABE_DPVS_PUBLIC_KEY public_key = kpabe.get_public_key();
  KPABE_DPVS_MASTER_KEY master_key = kpabe.get_master_key();

  cout << "\nBenchmarking serialization of public and master keys\n" << endl;

  // serialization of public key
  duration<double, std::micro> duration_pk(0);
  for (int i = 0; i < rounds; i++) {
    ByteString public_key_bytes;
  
    auto t1 = high_resolution_clock::now();
    public_key.serialize(public_key_bytes);
    auto t2 = high_resolution_clock::now();

    duration_pk += duration_cast<microseconds>( t2 - t1 );
  }
  cout << "Serialization of public key --> ByteString: " << duration_pk.count() / rounds << " us" << endl;

  // serialization of master key
  duration<double, std::micro> duration_mk(0);
  for (int i = 0; i < rounds; i++) {
    ByteString master_key_bytes;
  
    auto t1 = high_resolution_clock::now();
    master_key.serialize(master_key_bytes);
    auto t2 = high_resolution_clock::now();

    duration_mk += duration_cast<microseconds>( t2 - t1 );
  }
  cout << "Serialization of master key --> ByteString: " << duration_mk.count() / rounds << " us" << endl;
}
