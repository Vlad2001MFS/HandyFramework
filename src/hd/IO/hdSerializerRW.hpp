#pragma once
#include "hdSerializer.hpp"
#include "hdDeserializer.hpp"

namespace hd {

class SerializerRW {
public:
    SerializerRW();
    SerializerRW(const std::string &filename, bool serialize);
    ~SerializerRW();

    void create(const std::string &filename, bool serialize);
    void destroy();

    template<typename T>
    void process(const T &value) {
        HD_ASSERT(mSerializer);
        mSerializer->write<T>(value);
    }

    template<typename T>
    void process(T &value) {
        HD_ASSERT(mSerializer || mDeserializer);
        if (mSerializer) {
            mSerializer->write<T>(value);
        }
        else {
            mDeserializer->read<T>(value);
        }
    }

    template<typename T>
    T process() {
        HD_ASSERT(mDeserializer);
        return mDeserializer->read<T>();
    }

    template<typename T>
    void processVector(std::vector<T> &v) {
        HD_ASSERT(mSerializer || mDeserializer);
        if (mSerializer) {
            process<uint64_t>(v.size());
            processBuffer(v.data(), sizeof(T)*v.size());
        }
        else {
            v.resize(process<uint64_t>());
            processBuffer(v.data(), sizeof(T)*v.size());
        }
    }

    void processBuffer(void *buffer, size_t size);
    void processString(const std::string &value);
    void processString(std::string &value);
    std::string processString();

    bool isRead() const { return mDeserializer != nullptr; }
    bool isWrite() const { return mSerializer != nullptr; }

private:
    Serializer *mSerializer;
    Deserializer *mDeserializer;
};

}
