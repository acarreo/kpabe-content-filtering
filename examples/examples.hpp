/**
 * @file examples.hpp
 * @author Adam Oumar Abdel-rahman
 * @brief Examples for the KP-ABE scheme using DPVS
 * @date 2023-08-20
 *
 */

#ifndef __EXAMPLES_HPP__
#define __EXAMPLES_HPP__

#include <iostream>
#include <fstream>

#include "../kpabe/kpabe.hpp"

/* Generate public and private params and export theme */
void generate_params();

/* Generate a decryption key for a user, and export it */
void generate_decryption_key(std::string policy, std::string master_key_file,
                             std::string dec_key_file);

/* Encrypt a session key and export the ciphertext */
void encrypt_and_export(uint8_t* session_key, std::string url,
                        std::string attributes, std::string public_key_file,
                        std::string ciphertext_file);

/* Decrypt a ciphertext and recover the session key */
void decrypt(uint8_t* session_key, std::string ciphertext_file,
             std::string dec_key_file);

void example_generate_keys();

void example_encryption();

#endif // __EXAMPLES_HPP__
