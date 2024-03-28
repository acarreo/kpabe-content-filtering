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

void bench_serialization_params(int rounds = 1000);     // public and master keys
void bench_serialization_dec_key(int rounds = 1000);    // decryption key
void bench_encryption(int rounds = 1000);
void bench_decryption(int rounds = 1000);


#endif // __NEW_BENCH_HPP__
