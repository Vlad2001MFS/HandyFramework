#pragma once
#include "Stream.hpp"
#include <cstdio>

namespace hd {

enum class FileMode {
    Read,
    Write,
    ReadWrite
};

class FileStream : public Stream {
public:
    FileStream();
    FileStream(const std::string &path, FileMode mode);
    ~FileStream() override;

    size_t read(void *data, size_t size) override;
    size_t write(const void *data, size_t size) override;
    size_t tell() const override;
    size_t getSize() const override;
    bool seek(size_t pos) override;
    bool isEOF() const override;
    bool isReadable() const override;
    bool isWritable() const override;

    void create(const std::string &path, FileMode mode);
    void destroy();

    using Stream::read;
    using Stream::write;

private:
    FILE *mFile;
    FileMode mMode;

};

}
