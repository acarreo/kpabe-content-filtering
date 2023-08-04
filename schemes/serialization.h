#ifndef __SERIALIZATION_H__
#define __SERIALIZATION_H__

#include <msgpack.hpp>
#include <iostream>
#include <string>
#include <map>

// #include "kpabe.h"
#include "../dpvs/dpvs.h"

struct vect_2d_uint8_t {
    std::vector<std::vector<uint8_t>> data;

    MSGPACK_DEFINE(data);
};


// Serialization of g1_vect_t and g2_vect_t
void serialize_g1_vect (const G1_VS_VECT vect1, std::vector<uint8_t>& packed_data);
void serialize_g2_vect (const G2_VS_VECT vect2, std::vector<uint8_t>& packed_data);

// Deserialization of g1_vect_t and g2_vect_t
G1_VS_VECT deserialize_g1_vect(const std::vector<uint8_t>& packed_data);
G2_VS_VECT deserialize_g2_vect(const std::vector<uint8_t>& packed_data);

#endif // ifndef __SERIALIZATION_H__