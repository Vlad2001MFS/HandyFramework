#pragma once
#include "hdFileStream.hpp"

namespace hd {

class Deserializer {
public:
    Deserializer();
    explicit Deserializer(hd::StreamReader &stream);
    explicit Deserializer(const std::string &filename);
    ~Deserializer();

    void create(hd::StreamReader &stream);
    void create(const std::string &filename);
    void destroy();

    void readString(std::string &value);
    std::string readString();

    template<typename T>
    void read(T &value) {
        HD_ASSERT(mStream);
        mStream->read<T>(value);
    }

    template<typename T>
    T read() {
        T value;
        read<T>(value);
        return value;
    }

private:
    hd::StreamReader *mStream;
    hd::FileReader mFileStream;
};

}
