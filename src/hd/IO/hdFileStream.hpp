#pragma once
#include "hdStream.hpp"

typedef FILE FILE;

namespace hd {

class FileReader : public StreamReader {
public:
    FileReader();
    explicit FileReader(const std::string &filename);
    ~FileReader();

    bool isEOF() const;
    bool setPosition(int offset, StreamOrigin origin);
    uint32_t getPosition() const;
    size_t read(void *data, size_t size);
    uint32_t getSize() const;

    void open(const std::string &filename);
    void close();
    FILE *getHandle() const { return mHandle; }

    using StreamReader::read;

private:
    FILE *mHandle;
};

class FileWriter : public StreamWriter {
public:
    FileWriter();
    explicit FileWriter(const std::string &filename);
    ~FileWriter();

    bool isEOF() const;
    bool setPosition(int offset, StreamOrigin origin);
    uint32_t getPosition() const;
    size_t write(const void *data, size_t size);
    uint32_t getSize() const;

    void open(const std::string &filename);
    void close();
    void flush();
    FILE *getHandle() const { return mHandle; }

    using StreamWriter::write;

private:
    FILE *mHandle;
};

}
