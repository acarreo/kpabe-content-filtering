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
  bench_serialization_params_iostream();
  bench_serialization_params(BENCH_ROUNDS, BIN_UNCOMPRESSED);
  bench_serialization_params_iostream(BENCH_ROUNDS, BIN_UNCOMPRESSED);
  // bench_serialization_dec_key();
  // bench_encryption();
  // bench_decryption();


  clean_libraries();
  return 0;
}

void bench_serialization_params(int rounds, CompressionType compress)
{
  cout << "\n----------------> START : " << __func__ << endl;
  cout << "Benchmarking serialization of public and master keys -- Compression = " << compress << "\n" << endl;

  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    std::cerr << "Error: Could not setup keys" << std::endl;
    return;
  }

  KPABE_DPVS_PUBLIC_KEY public_key = kpabe.get_public_key();
  KPABE_DPVS_MASTER_KEY master_key = kpabe.get_master_key();


  // serialization of public key
  duration<double, std::micro> ser_pk_duration(0);
  duration<double, std::micro> ser_mk_duration(0);
  duration<double, std::micro> des_pk_duration(0);
  duration<double, std::micro> des_mk_duration(0);

  chrono::time_point<chrono::high_resolution_clock> t1, t2, t3;

  for (int i = 0; i < rounds; i++) {
    ByteString public_key_bytes;
    ByteString master_key_bytes;

    KPABE_DPVS_PUBLIC_KEY public_key2;
    KPABE_DPVS_MASTER_KEY master_key2;

    t1 = high_resolution_clock::now();
    public_key.serialize(public_key_bytes, compress);
    t2 = high_resolution_clock::now();
    public_key2.deserialize(public_key_bytes);
    t3 = high_resolution_clock::now();

    ser_pk_duration += duration_cast<microseconds>( t2 - t1 );
    des_pk_duration += duration_cast<microseconds>( t3 - t2 );
  
    t1 = high_resolution_clock::now();
    master_key.serialize(master_key_bytes, compress);
    t2 = high_resolution_clock::now();
    master_key2.deserialize(master_key_bytes);
    t3 = high_resolution_clock::now();

    ser_mk_duration += duration_cast<microseconds>( t2 - t1 );
    des_mk_duration += duration_cast<microseconds>( t3 - t2 );

    if (public_key != public_key2 || master_key != master_key2) {
      std::cerr << "Error: Public and master keys are not equal" << std::endl;
      return;
    }
  }
  cout << "Serialization of public key --> ByteString: " << (int) ser_pk_duration.count() / rounds << " us" << endl;
  cout << "Serialization of master key --> ByteString: " << (int) ser_mk_duration.count() / rounds << " us" << endl;
  cout << "-----------------------------------------------------------------------" << endl;
  cout << "Deserialization of public key <-- ByteString: " << (int) des_pk_duration.count() / rounds << " us" << endl;
  cout << "Deserialization of master key <-- ByteString: " << (int) des_mk_duration.count() / rounds << " us" << endl;

  cout << "Size of public key: " << public_key.getSizeInBytes(compress) << " bytes" << endl;
  cout << "Size of master key: " << master_key.getSizeInBytes(compress) << " bytes" << endl;
}


void bench_serialization_params_iostream(int rounds, CompressionType compress)
{
  cout << "\n----------------> START : " << __func__ << endl;
  cout << "Benchmarking serialization of public and master keys -- Compression = " << compress << "\n" << endl;

  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    std::cerr << "Error: Could not setup keys" << std::endl;
    return;
  }

  KPABE_DPVS_PUBLIC_KEY public_key = kpabe.get_public_key();
  KPABE_DPVS_MASTER_KEY master_key = kpabe.get_master_key();

  duration<double, std::micro> ser_pk_duration(0);
  duration<double, std::micro> ser_mk_duration(0);
  duration<double, std::micro> des_pk_duration(0);
  duration<double, std::micro> des_mk_duration(0);

  chrono::time_point<chrono::high_resolution_clock> t1, t2, t3;

  for (int i = 0; i < rounds; i++) {
    stringstream ss_public_key, ss_master_key;

    KPABE_DPVS_PUBLIC_KEY public_key2;
    KPABE_DPVS_MASTER_KEY master_key2;

    t1 = high_resolution_clock::now();
    public_key.serialize(ss_public_key, compress);
    t2 = high_resolution_clock::now();
    public_key2.deserialize(ss_public_key);
    t3 = high_resolution_clock::now();

    ser_pk_duration += duration_cast<microseconds>( t2 - t1 );
    des_pk_duration += duration_cast<microseconds>( t3 - t2 );

    t1 = high_resolution_clock::now();
    master_key.serialize(ss_master_key, compress);
    t2 = high_resolution_clock::now();
    master_key2.deserialize(ss_master_key);
    t3 = high_resolution_clock::now();

    ser_mk_duration += duration_cast<microseconds>( t2 - t1 );
    des_mk_duration += duration_cast<microseconds>( t3 - t2 );

    if (public_key != public_key2 || master_key != master_key2) {
      std::cerr << "Error: Public and master keys are not equal" << std::endl;
      return;
    }
  }
  cout << "Serialization of public key --> iostream: " << (int) ser_pk_duration.count() / rounds << " us" << endl;
  cout << "Serialization of master key --> iostream: " << (int) ser_mk_duration.count() / rounds << " us" << endl;
  cout << "-----------------------------------------------------------------------" << endl;
  cout << "Deserialization of public key <-- iostream: " << (int) des_pk_duration.count() / rounds << " us" << endl;
  cout << "Deserialization of master key <-- iostream: " << (int) des_mk_duration.count() / rounds << " us" << endl;

  cout << "Size of public key: " << public_key.getSizeInBytes(compress) << " bytes" << endl;
  cout << "Size of master key: " << master_key.getSizeInBytes(compress) << " bytes" << endl;
}
