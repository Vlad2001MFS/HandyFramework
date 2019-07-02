#include "hdSerializer.hpp"

namespace hd {

Serializer::Serializer() {
    mStream = nullptr;
}

Serializer::Serializer(hd::StreamWriter &stream) : Serializer() {
	create(stream);
}

Serializer::Serializer(const std::string &filename) : Serializer() {
    create(filename);
}

Serializer::~Serializer() {
	destroy();
}

void Serializer::create(hd::StreamWriter &stream) {
	destroy();
    mStream = &stream;
}

void Serializer::create(const std::string &filename) {
	destroy();
    mFileStream.open(filename);
    mStream = &mFileStream;
}

void Serializer::destroy() {
    mFileStream.close();
    mStream = nullptr;
}

void Serializer::writeString(const std::string &value) {
    uint64_t strLen = value.length();
    const char *strData = value.data();
    mStream->write(strLen);
    mStream->write(strData, sizeof(char)*strLen);
}
    
}
