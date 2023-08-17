#include "serialization.h"
#include "../dpvs/dpvs.h"
#include "../utils.h"

using namespace std;

// Serialization of g1_vect_t
void serialize_g1_vect(const G1_VS_VECT vect, vector<uint8_t>& packed_data) {
    vect_2d_uint8_t vect_bin;
    for (uint8_t i = 0; i < vect->dim; i++) {
        vector<uint8_t> g1_bin;
        int size = g1_size_bin(vect->coord[i], 0);
        uint8_t* buffer = (uint8_t*) malloc(size);
        g1_write_bin(buffer, size, vect->coord[i], 0);
        g1_bin.assign(buffer, buffer + size);
        free(buffer);
        vect_bin.data.push_back(g1_bin);
    }

    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, vect_bin.data);

    packed_data.assign(sbuf.data(), sbuf.data() + sbuf.size());
}

// Serialization of g2_vect_t
void serialize_g2_vect(const G2_VS_VECT vect, vector<uint8_t>& packed_data) {
    vect_2d_uint8_t vect_bin;
    for (uint8_t i = 0; i < vect->dim; i++) {
        vector<uint8_t> g2_bin;
        int size = g2_size_bin(vect->coord[i], 0);
        uint8_t* buffer = (uint8_t*) malloc(size);
        g2_write_bin(buffer, size, vect->coord[i], 0);
        g2_bin.assign(buffer, buffer + size);
        free(buffer);
        vect_bin.data.push_back(g2_bin);
    }

    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, vect_bin.data);

    packed_data.assign(sbuf.data(), sbuf.data() + sbuf.size());
}

// Deserialization of g1_vect_t
G1_VS_VECT deserialize_g1_vect(const vector<uint8_t>& packed_data) {
    msgpack::object_handle oh = msgpack::unpack(reinterpret_cast<const char*>(packed_data.data()), packed_data.size());
    msgpack::object obj = oh.get();

    vect_2d_uint8_t vect_bin;
    obj.convert(vect_bin.data);
    uint8_t dim = vect_bin.data.size();

    G1_VS_VECT vect_out = (G1_VS_VECT)dpvs_create_g1_vect(dim);
    if (vect_out != NULL) {
        for (uint8_t i = 0; i < vect_out->dim; i++) {
            g1_read_bin(vect_out->coord[i], vect_bin.data[i].data(), vect_bin.data[i].size());
        }
    }

    return vect_out;
}

// Deserialization of g2_vect_t
G2_VS_VECT deserialize_g2_vect(const vector<uint8_t>& packed_data) {
    msgpack::object_handle oh = msgpack::unpack(reinterpret_cast<const char*>(packed_data.data()), packed_data.size());
    msgpack::object obj = oh.get();

    vect_2d_uint8_t vect_bin;
    obj.convert(vect_bin.data);
    uint8_t dim = vect_bin.data.size();
    G2_VS_VECT vect_out = (G2_VS_VECT)dpvs_create_g2_vect(dim);

    if (vect_out != NULL) {
        for (uint8_t i = 0; i < vect_out->dim; i++) {
            g2_read_bin(vect_out->coord[i], vect_bin.data[i].data(), vect_bin.data[i].size());
        }
    }

    return vect_out;
}
