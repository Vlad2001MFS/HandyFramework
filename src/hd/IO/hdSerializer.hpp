#pragma once
#include "hdFileStream.hpp"

namespace hd {

class Serializer {
public:
    Serializer();
    explicit Serializer(hd::StreamWriter &stream);
    explicit Serializer(const std::string &filename);
    ~Serializer();

    void create(hd::StreamWriter &stream);
    void create(const std::string &filename);
    void destroy();

    void writeString(const std::string &value);

    template<typename T>
    void write(const T &value) {
        HD_ASSERT(mStream);
        mStream->write<T>(value);
    }

private:
    hd::StreamWriter *mStream;
    hd::FileWriter mFileStream;
};

}
