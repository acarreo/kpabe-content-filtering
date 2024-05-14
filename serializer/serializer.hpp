#ifndef __SERIALIZER_HPP__
#define __SERIALIZER_HPP__

#include <lsss_abe/lsss_abe.h>
#include <iostream>
#include <fstream>
#include <vector>


using ByteString = OpenABEByteString;

bool getSizeFromStream(std::istream &is, size_t *size, ByteString &size_buf);

template <class T>
class Serializer {
  public:
    void serializeToBuffer(std::vector<uint8_t> &buffer) const {
      ByteString temp;
      static_cast<const T*>(this)->serialize(temp);
      buffer.resize(temp.size());
      std::copy(temp.data(), temp.data() + temp.size(), buffer.begin());
    }

    void deserializeFromBuffer(const std::vector<uint8_t> &buffer) {
      ByteString temp;
      temp.fillBuffer(0, buffer.size());
      std::copy(buffer.begin(), buffer.end(), temp.data());
      static_cast<T*>(this)->deserialize(temp);
    }

    void serializeToStream(std::ostream& os) const {
      ByteString temp;
      static_cast<const T*>(this)->serialize(temp);
      os.write(reinterpret_cast<const char*>(temp.data()), static_cast<std::streamsize>(temp.size()));
    }

    void deserializeFromStream(std::istream& is) {
      if (is.good()) {
        ByteString temp, bytes;
        size_t size = 0;

        if (!getSizeFromStream(is, &size, bytes)) {
          std::cerr << "Error: Could not get size from stream" << std::endl;
          return;
        }

        temp.fillBuffer(0, size);
        is.read(reinterpret_cast<char*>(temp.getInternalPtr()), static_cast<std::streamsize>(size));
        if (!is.good()) {
          std::cerr << "Error: Could not read data" << std::endl;
          return;
        }

        bytes += temp;
        static_cast<T*>(this)->deserialize(bytes);
      }
    }
};

#endif // __SERIALIZER_HPP__
