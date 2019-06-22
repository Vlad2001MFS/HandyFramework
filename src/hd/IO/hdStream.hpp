#pragma once
#include "../Core/hdCommon.hpp"
#include <string>

namespace hd {

enum class StreamOrigin {
    Begin,
    Cursor,
    End
};

class Stream {
    HD_NONCOPYABLE_CLASS(Stream)
public:
    Stream();
    virtual ~Stream();

    virtual bool isEOF() const = 0;
    virtual bool setPosition(int offset, StreamOrigin origin) = 0;
    virtual uint32_t getPosition() const = 0;
    virtual uint32_t getSize() const = 0;

    void setName(const std::string &name) { mName = name; }
    const std::string &getName() const { return mName; }

private:
    std::string mName;
};

class StreamReader : public Stream {
public:
    virtual ~StreamReader();

    virtual size_t read(void *data, size_t size) = 0;

    template<typename T>
    size_t read(T &value) {
        return read(&value, sizeof(value));
    }
};

class StreamWriter : public Stream {
public:
    virtual ~StreamWriter();

    virtual size_t write(const void *data, size_t size) = 0;

    template<typename T>
    size_t write(const T &value) {
        return write(&value, sizeof(value));
    }
};

}
