#include "hdDeserializer.hpp"

namespace hd {

Deserializer::Deserializer() {
    mStream = nullptr;
}

Deserializer::Deserializer(hd::StreamReader &stream) : Deserializer() {
	create(stream);
}

Deserializer::Deserializer(const std::string &filename) : Deserializer() {
    create(filename);
}

Deserializer::~Deserializer() {
	destroy();
}

void Deserializer::create(hd::StreamReader &stream) {
	destroy();
    mStream = &stream;
}

void Deserializer::create(const std::string &filename) {
	destroy();
    mFileStream.open(filename);
    mStream = &mFileStream;
}

void Deserializer::destroy() {
    mFileStream.close();
	mStream = nullptr;
}

void Deserializer::readBuffer(void *data, size_t size) {
    HD_ASSERT(data);
    HD_ASSERT(size);
    HD_ASSERT(mStream->read(data, size) == size);
}

void Deserializer::readString(std::string &value) {
    uint64_t strLen;
    mStream->read(strLen);
    value.resize(strLen, '\0');
    mStream->read(value.data(), sizeof(char)*strLen);
}

std::string Deserializer::readString() {
    std::string value;
    readString(value);
    return value;
}
    
}
