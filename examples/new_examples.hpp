/**
 * @file new_examples.hpp
 * @author Adam Oumar Abdel-rahman
 * @brief New examples for the KP-ABE scheme using DPVS
 * @date 2024-03-23
 *
 */

#ifndef __NEW_EXAMPLES_HPP__
#define __NEW_EXAMPLES_HPP__

#include <iostream>
#include <fstream>

#include "../kpabe/kpabe.hpp"

void example_generate_params();
void example_generate_decryption_key();
void example_encrypt_decrypt();
void example_encrypt_decrypt_old();
void example_unlinkability_public_keys();
void example_unlinkability_decryption();
void example_serialization();


#endif // __NEW_EXAMPLES_HPP__
