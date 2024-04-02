/**
 * @file new_bench.hpp
 * @author Adam Oumar Abdel-rahman
 * @brief 
 * @date 2024-03-27
 *
 */

#ifndef __NEW_BENCH_HPP__
#define __NEW_BENCH_HPP__

#include "../kpabe/kpabe.hpp"

#define BENCH_ROUNDS 1000

void bench_serialization_params(int rounds = BENCH_ROUNDS, CompressionType compress = BIN_COMPRESSED);
void bench_serialization_params_iostream(int rounds = BENCH_ROUNDS, CompressionType compress = BIN_COMPRESSED);
void bench_serialization_dec_key(int rounds = BENCH_ROUNDS);
void bench_encryption(int rounds = BENCH_ROUNDS);
void bench_decryption(int rounds = BENCH_ROUNDS);


#endif // __NEW_BENCH_HPP__
