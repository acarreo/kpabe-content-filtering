// #include "serializer.hpp"


// template <class T>
// void Serializer<T>::serializeToBuffer(std::vector<uint8_t> &buffer) const {
// 	ByteString temp;
// 	static_cast<const T*>(this)->serialize(temp, true);
// 	buffer.resize(temp.size());
// 	std::copy(temp.data(), temp.data() + temp.size(), buffer.begin());
// }

// template <class T>
// void Serializer<T>::deserializeFromBuffer(const std::vector<uint8_t> &buffer) {
// 	ByteString temp;
// 	temp.fillBuffer(0, buffer.size());
// 	std::copy(buffer.begin(), buffer.end(), temp.data());
// 	static_cast<T*>(this)->deserialize(temp);
// }
