#include "hdSerializerRW.hpp"

namespace hd {

SerializerRW::SerializerRW() {
    mSerializer = nullptr;
    mDeserializer = nullptr;
}

SerializerRW::SerializerRW(const std::string &filename, bool serialize) : SerializerRW() {
    create(filename, serialize);
}

SerializerRW::~SerializerRW() {
    destroy();
}

void SerializerRW::create(const std::string &filename, bool serialize) {
    destroy();
    if (serialize) {
        mSerializer = new Serializer(filename);
    }
    else {
        mDeserializer = new Deserializer(filename);
    }
}

void SerializerRW::destroy() {
    HD_DELETE(mSerializer);
    HD_DELETE(mDeserializer);
}

void SerializerRW::processString(const std::string &value) {
    HD_ASSERT(mSerializer);
    mSerializer->writeString(value);
}

void SerializerRW::processString(std::string &value) {
    HD_ASSERT(mSerializer || mDeserializer);
    if (mSerializer) {
        mSerializer->writeString(value);
    }
    else {
        mDeserializer->readString(value);
    }
}

std::string SerializerRW::processString() {
    HD_ASSERT(mDeserializer);
    return mDeserializer->readString();
}

}
