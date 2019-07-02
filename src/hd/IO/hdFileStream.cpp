#include "hdFileStream.hpp"
#include <stdio.h>

namespace hd {
    
static const int gOrigins[] = {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

FileReader::FileReader() {
    mHandle = nullptr;
}

FileReader::FileReader(const std::string &filename) : FileReader() {
	open(filename);
}

FileReader::~FileReader() {
	close();
}

bool FileReader::isEOF() const {
	return mHandle && feof(mHandle);
}

bool FileReader::setPosition(int offset, StreamOrigin origin) {
	HD_ASSERT(mHandle);
    return fseek(mHandle, offset, gOrigins[static_cast<size_t>(origin)]) == 0;
}

uint32_t FileReader::getPosition() const {
	HD_ASSERT(mHandle);
    uint32_t pos = ftell(mHandle);
    if (pos < 0) {
        HD_LOG_ERROR("Failed to get position of stream '%s'", getName().data());
    }
    return pos;
}

size_t FileReader::read(void *data, size_t size) {
    HD_ASSERT(!isEOF());
	return fread(data, 1, size, mHandle);
}

uint32_t FileReader::getSize() const {
    uint32_t curPos = getPosition();
    fseek(mHandle, 0, SEEK_END);
    uint32_t size = getPosition();
    fseek(mHandle, static_cast<int>(curPos), SEEK_SET);
    return size;
}

void FileReader::open(const std::string &filename) {
    HD_ASSERT(!filename.empty());
	close();
    mHandle = fopen(filename.data(), "rb");
    if (!mHandle) {
        HD_LOG_ERROR("Failed to open file stream reader '%s'", filename.data());
    }
    setName(filename);
}

void FileReader::close() {
	if (mHandle) {
        fclose(mHandle);
        mHandle = nullptr;
    }
}



FileWriter::FileWriter() {
    mHandle = nullptr;
}

FileWriter::FileWriter(const std::string &filename) : FileWriter() {
	open(filename);
}

FileWriter::~FileWriter() {
	close();
}

bool FileWriter::isEOF() const {
	return mHandle && feof(mHandle);
}

bool FileWriter::setPosition(int offset, StreamOrigin origin) {
	HD_ASSERT(mHandle);
    return fseek(mHandle, offset, gOrigins[static_cast<size_t>(origin)]) == 0;
}

uint32_t FileWriter::getPosition() const {
    HD_ASSERT(mHandle);
    uint32_t pos = ftell(mHandle);
    if (pos < 0) {
        HD_LOG_ERROR("Failed to get position of stream '%s'", getName().data());
    }
    return pos;
}

size_t FileWriter::write(const void *data, size_t size) {
	HD_ASSERT(mHandle);
	return fwrite(data, 1, size, mHandle);
}

uint32_t FileWriter::getSize() const {
    uint32_t curPos = getPosition();
    fseek(mHandle, 0, SEEK_END);
    uint32_t size = getPosition();
    fseek(mHandle, static_cast<int>(curPos), SEEK_SET);
    return size;
}

void FileWriter::open(const std::string &filename) {
    HD_ASSERT(!filename.empty());
	close();
    mHandle = fopen(filename.data(), "wb");
    if (!mHandle) {
        HD_LOG_ERROR("Failed to open file stream writer '%s'", filename.data());
    }
    setName(filename);
}

void FileWriter::close() {
	if (mHandle) {
        fclose(mHandle);
        mHandle = nullptr;
    }
}

void FileWriter::flush() {
	HD_ASSERT(mHandle);
    fflush(mHandle);
}

}
