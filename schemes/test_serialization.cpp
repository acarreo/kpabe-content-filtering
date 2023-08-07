#include <iostream>
#include <iomanip>
#include <vector>
#include "serialization.h"
#include "../dpvs/dpvs.h"
#include "../utils.h"

group_setting_t params;
bn_t Fq;

// void random_vect_g1 (G1_VS_VECT& vect) {
//     for (uint8_t i = 0; i < vect->dim; i++) {
//         g1_rand(vect->coord[i]);
//     }
// }

// compare two g1_vect_t
bool compare_g1_vect(const G1_VS_VECT& vect1, const G1_VS_VECT& vect2) {
    if (vect1->dim != vect2->dim) {
        return false;
    }

    for (uint8_t i = 0; i < vect1->dim; i++) {
        if (g1_cmp(vect1->coord[i], vect2->coord[i]) != RLC_EQ) {
            return false;
        }
    }

    return true;
}

bool compare_g2_vect(const G2_VS_VECT& vect1, const G2_VS_VECT& vect2) {
    if (vect1->dim != vect2->dim) {
        return false;
    }

    for (uint8_t i = 0; i < vect1->dim; i++) {
        if (g2_cmp(vect1->coord[i], vect2->coord[i]) != RLC_EQ) {
            return false;
        }
    }

    return true;
}

void print_hexdump(const std::vector<uint8_t>& data) {
    const int width = 16; // Nombre d'octets à afficher par ligne
    size_t size = data.size();

    for (size_t i = 0; i < size; i += width) {
        std::cout << std::setw(8) << std::setfill('0') << std::hex << i << "  ";
        for (size_t j = 0; j < width; j++) {
            if (i + j < size) {
                std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(data[i + j]) << " ";
            } else {
                std::cout << "   ";
            }
        }
        std::cout << " ";
        for (size_t j = 0; j < width; j++) {
            if (i + j < size) {
                char ch = isprint(data[i + j]) ? static_cast<char>(data[i + j]) : '.';
                std::cout << ch;
            } else {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}

int main() {

    if (!init_libraries()) {
        std::cout << "Erreur lors de l'initialisation des bibliothèques" << std::endl;
        return 1;
    }

#if 0
    dpvs_t* dpvs = generate_dpvs_bases(2);
    if (dpvs == nullptr) {
        std::cout << "Erreur lors de la génération des bases DPVS" << std::endl;
        return 1;
    }

    G1_VS_BASE base = dpvs->base;
    G2_VS_BASE dual_base = dpvs->dual_base;

    // Sérialisation de vect1[0] et vect2[0]
    std::vector<uint8_t> serialized_data_vect1;
    std::vector<uint8_t> serialized_data_vect2;

    serialize_g1_vect(base[0], serialized_data_vect1);
    serialize_g2_vect(dual_base[0], serialized_data_vect2);

    // Désérialisation des données sérialisées en une nouvelle instance de G1_VS_VECT et G2_VS_VECT
    G1_VS_VECT vect1_deserialized = deserialize_g1_vect(serialized_data_vect1);
    G2_VS_VECT vect2_deserialized = deserialize_g2_vect(serialized_data_vect2);

    // // Check if deserialized data is the same as the original data
    if (!compare_g1_vect(base[0], vect1_deserialized) ||
        !compare_g2_vect(dual_base[0], vect2_deserialized)) {
        std::cout << "Erreur lors de la sérialisation/désérialisation" << std::endl;
    }
    else {
        std::cout << "Sérialisation/désérialisation réussie" << std::endl;
        // print_vect_base(vect1_deserialized);
    }


    dpvs_clear_base_vect(vect1_deserialized);
    dpvs_clear_dual_base_vect(vect2_deserialized);

    dpvs_clear(dpvs);
#endif
    clean_libraries();

    return 0;
}
