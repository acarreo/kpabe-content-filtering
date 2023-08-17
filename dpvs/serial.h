/**
 * @file serial.h
 * @author Adam Oumar Abdel-rahman
 * @brief Serialization of DPVS structures
 * @date 2023-08-08
 * 
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "dpvs.h"
#include "../utils.h"

#define G1_SIZE_BIN     (RLC_PC_BYTES * 2 + 1)
#define G2_SIZE_BIN     (RLC_PC_BYTES * 4 + 1)


int serialize_g1_element(const g1_t g1, uint8_t buf[]);
int deserialize_g1_element(const uint8_t buf[], g1_t g1);

int serialize_g2_element(const g2_t g2, uint8_t buf[]);
int deserialize_g2_element(const uint8_t buf[], g2_t g2);

int serialize_g1_vector(const G1_VS_VECT vect, uint8_t** buf_serialized, int* size);
int deserialize_g1_vector(const uint8_t* buf, const int size, g1_vect_t** deserialized_vector); 

int serialize_g2_vector(const G2_VS_VECT vect, uint8_t** buf_serialized, int* size);
int deserialize_g2_vector(const uint8_t* buf, const int size, g2_vect_t** deserialized_vector);

#endif // __SERIAL_H__