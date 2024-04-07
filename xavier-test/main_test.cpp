#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <sstream>
#include <vector>
#include <iomanip>

// #include "kpabe-content-filtering/dpvs/vector_ec.hpp"
// #include "kpabe-content-filtering/keys/keys.hpp"
#include "kpabe_utils.h"
#include "ssl_utils.h"

std::vector<unsigned char> copy_raw_data_with_size(const std::vector<unsigned char> &raw_data);
void print_raw_data(const std::vector<unsigned char> &raw_data);

bn_t Fq;
int main() {
    if (!init_libraries()) {
        std::cerr << "unable to initialize the KP-ABE library" << std::endl;
        return 1;
    }

    std::cout << std::endl << "deserialize from JSON:" << std::endl << std::endl;

    std::fstream f("./json.txt");
    if (!f.good()) {
        std::cerr << "unable to open file" << std::endl;
        return 1;
    }

    std::vector<char> json_obj(std::istream_iterator<char>(f), std::istream_iterator<char>{});
    f.close();

    auto start = std::chrono::steady_clock::now();
    if (!nlohmann::json::accept(json_obj.begin(), json_obj.end())) {
        std::cerr << "message is not a valid Json" << std::endl;
        return 1;
    }

    auto json = nlohmann::json::parse(json_obj.begin(), json_obj.end());
    if (!json.contains("type")) {
        std::cerr << "message does not contains \"type\" entry" << std::endl;
        return 1;
    }

    std::cout << "parse JSON time = " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count()
              << " us" << std::endl
              << std::endl;
    const auto &data = json["data"];

    // public key
    {
        // public key base64 to raw
        start = std::chrono::steady_clock::now();
        std::vector<unsigned char> raw_data = base64_decode(data["public_key"]);
        std::cout << "base64 decode public key time = "
                  << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us" << std::endl;
        std::cout << "public key size = " << raw_data.size() << std::endl << std::endl;

        // copy raw data with size
        std::vector<unsigned char> raw_data_with_size = copy_raw_data_with_size(raw_data);
        std::cout << "public key size with size = " << raw_data_with_size.size() << std::endl << std::endl;

        // deserialize via istream
        {
            KPABE_DPVS_PUBLIC_KEY pub_key;
            start = std::chrono::steady_clock::now();
            IMemStream raw_data_stream(raw_data_with_size.data(), raw_data_with_size.size());
            pub_key.deserialize(raw_data_stream);
            std::cout << "deserialize public key via istream (no copy) time = "
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                      << std::endl;
            std::cout << "key size = " << pub_key.getSizeInBytes() << std::endl << std::endl;
        }
        // deserialize via stringstream (istream + copy)
        {
            KPABE_DPVS_PUBLIC_KEY pub_key;
            start = std::chrono::steady_clock::now();
            std::stringstream ss;
            ss.write((char *)raw_data_with_size.data(), raw_data_with_size.size());
            pub_key.deserialize(ss);
            std::cout << "deserialize public key via stringstream (with copy) time = "
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                      << std::endl;
            std::cout << "key size = " << pub_key.getSizeInBytes() << std::endl << std::endl;
        }
        // deserialize via vector
        {
            KPABE_DPVS_PUBLIC_KEY pub_key;
            start = std::chrono::steady_clock::now();
            pub_key.deserialize(raw_data);
            std::cout << "deserialize public key via vector (no copy) time = "
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                      << std::endl;
            std::cout << "key size = " << pub_key.getSizeInBytes() << std::endl << std::endl;
        }
        // deserialize via ByteString
        {
            KPABE_DPVS_PUBLIC_KEY pub_key;
            start = std::chrono::steady_clock::now();
            ByteString bytes;
            bytes.assign(raw_data.begin(), raw_data.end());
            pub_key.deserialize(bytes);
            std::cout << "deserialize public key via ByteString (with copy) time = "
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                      << std::endl;
            std::cout << "key size = " << pub_key.getSizeInBytes() << std::endl << std::endl;
        }
    }

    // decryption key
    {
        // public key base64 to raw
        start = std::chrono::steady_clock::now();
        std::vector<unsigned char> raw_data = base64_decode(data["decryption_key"]);
        std::cout << "base64 decode decryption key time = "
                  << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us" << std::endl;
        std::cout << "decryption key size = " << raw_data.size() << std::endl << std::endl;

        // copy raw data with size
        std::vector<unsigned char> raw_data_with_size = copy_raw_data_with_size(raw_data);
        std::cout << "decryption key size with size = " << raw_data_with_size.size() << std::endl << std::endl;

        // deserialize via istream
        {
            KPABE_DPVS_DECRYPTION_KEY dec_key;
            start = std::chrono::steady_clock::now();
            IMemStream raw_data_stream(raw_data_with_size.data(), raw_data_with_size.size());
            dec_key.deserialize(raw_data_stream);
            std::cout << "deserialize decryption key via istream time = "
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                      << std::endl;
            std::cout << "key size = " << dec_key.getSizeInBytes() << std::endl << std::endl;
        }
        // deserialize via vector
        {
            KPABE_DPVS_DECRYPTION_KEY dec_key;
            start = std::chrono::steady_clock::now();
            dec_key.deserialize(raw_data);
            std::cout << "deserialize decryption key via vector time = "
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                      << std::endl;
            std::cout << "key size = " << dec_key.getSizeInBytes() << std::endl << std::endl;
        }
    }

    std::cout << std::endl << "internal serialize/deserialize:" << std::endl << std::endl;

    // basic test
    {
        KPABE_DPVS kpabe;
        kpabe.setup();
        KPABE_DPVS_MASTER_KEY master_key = kpabe.get_master_key();
        KPABE_DPVS_PUBLIC_KEY pub_key = kpabe.get_public_key();
        std::vector<std::string> wl{"wikipedia.fr"};
        std::vector<std::string> bl{"facebook.com"};
        std::string policy = "pegi3 and pegi7 and pegi12";
        KPABE_DPVS_DECRYPTION_KEY dec_key(policy, wl, bl);
        if (!dec_key.generate(master_key)) {
            std::cerr << "fail to generate decryption key" << std::endl;
            return 1;
        }

        std::cout << "public key size = " << pub_key.getSizeInBytes() << std::endl;
        std::cout << "decryption key size = " << dec_key.getSizeInBytes() << std::endl << std::endl;

        // serialize public key via ByteString
        {
            start = std::chrono::steady_clock::now();
            ByteString raw_data;
            pub_key.serialize(raw_data);
            std::cout << "serialize public key via ByteString (no copy) time = "
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                      << std::endl;
            std::cout << "raw key size = " << raw_data.size() << std::endl << std::endl;

            unsigned char *network_bytes = raw_data.data();
            size_t network_bytes_size = raw_data.size();

            std::vector<unsigned char> raw_data_with_size = copy_raw_data_with_size(raw_data);
            unsigned char *network_bytes_with_size = raw_data_with_size.data();
            size_t size = raw_data_with_size.size();

            // deserialize via ByteString
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                ByteString raw_data;
                raw_data.assign(network_bytes, network_bytes + network_bytes_size);
                pub_key2.deserialize(raw_data);
                std::cout << "deserialize public key via ByteString (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }

            // deserialize via vector
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                std::vector<unsigned char> raw_data(network_bytes, network_bytes + network_bytes_size);
                pub_key2.deserialize(raw_data);
                std::cout << "deserialize public key via vector (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }

            // deserialize via stringstream
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                std::stringstream ss;
                ss.write((char *)network_bytes_with_size, size);
                pub_key2.deserialize(ss);
                std::cout << "deserialize public key via stringstream (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }

            // deserialize via istream, no copy
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                IMemStream ims(network_bytes_with_size, size);
                pub_key2.deserialize(ims);
                std::cout << "deserialize public key via custom istream (no copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }
        }

        std::cout << std::endl;

        // serialize public key via stringsteam (additionnal copy)
        {
            start = std::chrono::steady_clock::now();
            std::stringstream ss;
            pub_key.serialize(ss);
            std::string raw_data = ss.str();
            std::cout << "serialize public key via stringstream (with copy) time = "
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                      << std::endl;
            std::cout << "raw key size = " << raw_data.size() << std::endl << std::endl;

            unsigned char *network_bytes = (unsigned char *)raw_data.data();
            size_t network_bytes_size = raw_data.size();

            size_t size = 0;
            ss.read((char *)&size, sizeof(size));
            unsigned char *network_bytes_without_size = (unsigned char *)raw_data.data() + sizeof(size);

            // deserialize via ByteString
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                ByteString raw_data;
                raw_data.assign(network_bytes_without_size, network_bytes_without_size + size);
                pub_key2.deserialize(raw_data);
                std::cout << "deserialize public key via ByteString (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }

            // deserialize via vector
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                std::vector<unsigned char> raw_data(network_bytes_without_size, network_bytes_without_size + size);
                pub_key2.deserialize(raw_data);
                std::cout << "deserialize public key via vector (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }

            // deserialize via stringstream
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                std::stringstream ss;
                ss.write((char *)network_bytes, network_bytes_size);
                pub_key2.deserialize(ss);
                std::cout << "deserialize public key via stringstream (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }

            // deserialize via istream, no copy
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                IMemStream ims(network_bytes, network_bytes_size);
                pub_key2.deserialize(ims);
                std::cout << "deserialize public key via custom istream (without copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }
        }

        std::cout << std::endl;

        // serialize public key via custom ogsteam (no addtionnal copy)
        {
            start = std::chrono::steady_clock::now();
            std::vector<unsigned char> raw_data(pub_key.getSizeInBytes() + sizeof(size_t), 0);
            OMemStream oms(raw_data.data(), raw_data.size());
            pub_key.serialize(oms);
            std::cout << "serialize public key via custom ostream (no copy) time = "
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                      << std::endl;
            std::cout << "raw key size = " << raw_data.size() << std::endl << std::endl;

            unsigned char *network_bytes = raw_data.data();
            size_t network_bytes_size = raw_data.size();

            size_t size = raw_data.size() - sizeof(size_t);
            unsigned char *network_bytes_without_size = network_bytes + sizeof(size_t);

            // deserialize via ByteString
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                ByteString raw_data;
                raw_data.assign(network_bytes_without_size, network_bytes_without_size + size);
                pub_key2.deserialize(raw_data);
                std::cout << "deserialize public key via ByteString (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }

            // deserialize via vector
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                std::vector<unsigned char> raw_data(network_bytes_without_size, network_bytes_without_size + size);
                pub_key2.deserialize(raw_data);
                std::cout << "deserialize public key via vector (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }

            // deserialize via stringstream
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                std::stringstream ss;
                ss.write((char *)network_bytes, network_bytes_size);
                pub_key2.deserialize(ss);
                std::cout << "deserialize public key via stringstream (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }

            // deserialize via istream, no copy
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                IMemStream ims(network_bytes, network_bytes_size);
                pub_key2.deserialize(ims);
                std::cout << "deserialize public key via custom istream (without copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }
        }

        std::cout << std::endl;

        // serialize public key via oversized custom osteam (no addtionnal copy)
        {
            start = std::chrono::steady_clock::now();
            std::vector<unsigned char> raw_data(pub_key.getSizeInBytes() + 64, 0);
            OMemStream oms(raw_data.data(), raw_data.size());
            pub_key.serialize(oms);
            std::cout << "serialize public key via 'oversized' custom ostream (no copy) time = "
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                      << std::endl;
            std::cout << "raw key size = " << raw_data.size() << std::endl << std::endl;

            unsigned char *network_bytes = raw_data.data();
            size_t network_bytes_size = raw_data.size();

            // deserialize via ByteString
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                ByteString raw_data;
                raw_data.assign(network_bytes, network_bytes + network_bytes_size);
                pub_key2.deserialize(raw_data);
                std::cout << "deserialize public key via ByteString (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }

            // deserialize via vector
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                std::vector<unsigned char> raw_data(network_bytes, network_bytes + network_bytes_size);
                pub_key2.deserialize(raw_data);
                std::cout << "deserialize public key via vector (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }

            // deserialize via stringstream
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                std::stringstream ss;
                ss.write((char *)network_bytes, network_bytes_size);
                pub_key2.deserialize(ss);
                std::cout << "deserialize public key via stringstream (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }

            // deserialize via istream, no copy
            {
                KPABE_DPVS_PUBLIC_KEY pub_key2;
                start = std::chrono::steady_clock::now();
                IMemStream ims(network_bytes, network_bytes_size);
                pub_key2.deserialize(ims);
                std::cout << "deserialize public key via custom istream (without copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << pub_key2.getSizeInBytes() << ", equal = " << (pub_key == pub_key2) << std::endl << std::endl;
            }
        }

        std::cout << std::endl;

#if 0
        // serialize decryption key via ByteString
        {
            start = std::chrono::steady_clock::now();
            ByteString raw_data;
            dec_key.serialize(raw_data);
            std::cout << "serialize decryption key via ByteString (no copy) time = "
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                      << std::endl;
            std::cout << "raw key size = " << raw_data.size() << std::endl << std::endl;

            unsigned char *network_bytes = raw_data.data();
            size_t network_bytes_size = raw_data.size();

            // deserialize via ByteString
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                ByteString raw_data;
                raw_data.assign(network_bytes, network_bytes + network_bytes_size);
                dec_key2.deserialize(raw_data);
                std::cout << "deserialize decryption key via ByteString (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }

            // deserialize via vector
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                std::vector<unsigned char> raw_data(network_bytes, network_bytes + network_bytes_size);
                dec_key2.deserialize(raw_data);
                std::cout << "deserialize decryption key via vector (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }

            // deserialize via stringstream
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                std::stringstream ss;
                ss.write((char *)network_bytes, network_bytes_size);
                dec_key2.deserialize(ss);
                std::cout << "deserialize decryption key via stringstream (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }

            // deserialize via istream, no copy
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                IMemStream ims(network_bytes, network_bytes_size);
                dec_key2.deserialize(ims);
                std::cout << "deserialize decryption key via custom istream (without copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }
        }

        std::cout << std::endl;

        // serialize decryption key via stringsteam (additionnal copy)
        {
            start = std::chrono::steady_clock::now();
            std::stringstream ss;
            dec_key.serialize(ss);
            std::string raw_data = ss.str();
            std::cout << "serialize decryption key via stringstream (with copy) time = "
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                      << std::endl;
            std::cout << "raw key size = " << raw_data.size() << std::endl << std::endl;

            unsigned char *network_bytes = (unsigned char *)raw_data.data();
            size_t network_bytes_size = raw_data.size();

            // deserialize via ByteString
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                ByteString raw_data;
                raw_data.assign(network_bytes, network_bytes + network_bytes_size);
                dec_key2.deserialize(raw_data);
                std::cout << "deserialize decryption key via ByteString (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }

            // deserialize via vector
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                std::vector<unsigned char> raw_data(network_bytes, network_bytes + network_bytes_size);
                dec_key2.deserialize(raw_data);
                std::cout << "deserialize decryption key via vector (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }

            // deserialize via stringstream
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                std::stringstream ss;
                ss.write((char *)network_bytes, network_bytes_size);
                dec_key2.deserialize(ss);
                std::cout << "deserialize decryption key via stringstream (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }

            // deserialize via istream, no copy
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                IMemStream ims(network_bytes, network_bytes_size);
                dec_key2.deserialize(ims);
                std::cout << "deserialize decryption key via custom istream (without copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }
        }

        std::cout << std::endl;

        // serialize decryption key via custom ostream (no additionnal copy)
        {
            start = std::chrono::steady_clock::now();
            std::vector<unsigned char> raw_data(dec_key.getSizeInBytes(), 0);
            OMemStream oms(raw_data.data(), raw_data.size());
            dec_key.serialize(oms);
            std::cout << "serialize decryption key via custom ostream (with copy) time = "
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                      << std::endl;
            std::cout << "raw key size = " << raw_data.size() << std::endl << std::endl;

            unsigned char *network_bytes = raw_data.data();
            size_t network_bytes_size = raw_data.size();

            // deserialize via ByteString
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                ByteString raw_data;
                raw_data.assign(network_bytes, network_bytes + network_bytes_size);
                dec_key2.deserialize(raw_data);
                std::cout << "deserialize decryption key via ByteString (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }

            // deserialize via vector
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                std::vector<unsigned char> raw_data(network_bytes, network_bytes + network_bytes_size);
                dec_key2.deserialize(raw_data);
                std::cout << "deserialize decryption key via vector (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }

            // deserialize via stringstream
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                std::stringstream ss;
                ss.write((char *)network_bytes, network_bytes_size);
                dec_key2.deserialize(ss);
                std::cout << "deserialize decryption key via stringstream (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }

            // deserialize via istream, no copy
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                IMemStream ims(network_bytes, network_bytes_size);
                dec_key2.deserialize(ims);
                std::cout << "deserialize decryption key via custom istream (without copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }
        }

        std::cout << std::endl;

        // serialize decryption key via oversized custom ostream (no additionnal copy)
        {
            start = std::chrono::steady_clock::now();
            std::vector<unsigned char> raw_data(dec_key.getSizeInBytes() + 64, 0);
            OMemStream oms(raw_data.data(), raw_data.size());
            dec_key.serialize(oms);
            std::cout << "serialize decryption key via 'oversized' custom ostream (with copy) time = "
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                      << std::endl;
            std::cout << "raw key size = " << raw_data.size() << std::endl << std::endl;

            unsigned char *network_bytes = raw_data.data();
            size_t network_bytes_size = raw_data.size();

            // deserialize via ByteString
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                ByteString raw_data;
                raw_data.assign(network_bytes, network_bytes + network_bytes_size);
                dec_key2.deserialize(raw_data);
                std::cout << "deserialize decryption key via ByteString (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }

            // deserialize via vector
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                std::vector<unsigned char> raw_data(network_bytes, network_bytes + network_bytes_size);
                dec_key2.deserialize(raw_data);
                std::cout << "deserialize decryption key via vector (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }

            // deserialize via stringstream
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                std::stringstream ss;
                ss.write((char *)network_bytes, network_bytes_size);
                dec_key2.deserialize(ss);
                std::cout << "deserialize decryption key via stringstream (with copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }

            // deserialize via istream, no copy
            {
                KPABE_DPVS_DECRYPTION_KEY dec_key2;
                start = std::chrono::steady_clock::now();
                IMemStream ims(network_bytes, network_bytes_size);
                dec_key2.deserialize(ims);
                std::cout << "deserialize decryption key via custom istream (without copy) time = "
                          << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << " us"
                          << std::endl;
                std::cout << "key size = " << dec_key2.getSizeInBytes() << ", equal = " << (dec_key == dec_key2) << std::endl << std::endl;
            }
        }
#endif
    }

    return 0;
}


void print_raw_data(const std::vector<unsigned char> &raw_data) {
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    for (const auto &elem : raw_data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)elem;
    }
    std::cout << std::dec << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
}

// copy the raw data with a size_t at the beginning
std::vector<unsigned char> copy_raw_data_with_size(const std::vector<unsigned char> &raw_data) {
    std::vector<unsigned char> raw_data_with_size(raw_data.size() + sizeof(size_t), 0);
    size_t size = raw_data.size();
    memcpy(raw_data_with_size.data(), &size, sizeof(size_t));
    memcpy(raw_data_with_size.data() + sizeof(size_t), raw_data.data(), raw_data.size());
    return raw_data_with_size;
}
