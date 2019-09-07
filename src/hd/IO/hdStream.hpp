#pragma once
#include "../Core/hdMacros.hpp"
#include <string>

namespace hd {

class Stream {
    HD_NONCOPYABLE_CLASS(Stream);
public:
    Stream() = default;
    virtual ~Stream() = default;

    virtual size_t read(void *data, size_t size) = 0;
    virtual size_t write(const void *data, size_t size) = 0;
    virtual size_t tell() const = 0;
    virtual size_t getSize() const = 0;
    virtual bool seek(size_t pos) = 0;
    virtual bool isEOF() const = 0;
    virtual bool isReadable() const = 0;
    virtual bool isWritable() const = 0;

    std::string readLine(char separator);
    std::string readAllText();
    size_t writeLine(const std::string &line);

    void setName(const std::string &name);
    const std::string &getName() const;

    template<typename T>
    size_t read(T &value) {
        return read(&value, sizeof(T));
    }

    template<typename T>
    size_t write(const T &value) {
        return write(&value, sizeof(T));
    }

private:
    std::string mName;
};

}
