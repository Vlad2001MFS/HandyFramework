#include "hdFileStream.hpp"
#include "../Core/hdLog.hpp"
#include <cstdio>

namespace hd {
    
FileStream::FileStream() {
    mFile = nullptr;
    mMode = FileMode::Read;
}

FileStream::FileStream(const std::string &path, FileMode mode) : FileStream() {
    create(path, mode);
}

FileStream::~FileStream() {
    destroy();
}

size_t FileStream::read(void *data, size_t size) {
    HD_ASSERT(mFile);
    if (!isReadable()) {
        HD_LOG_ERROR("Failed to read %dz bytes from file '%s'", size, getName().data());
    }
    return fread(data, 1, size, mFile);
}

size_t FileStream::write(const void *data, size_t size) {
    HD_ASSERT(mFile);
    if (!isWritable()) {
        HD_LOG_ERROR("Failed to write %dz bytes to file '%s'", size, getName().data());
    }
    return fwrite(data, 1, size, mFile);
}

size_t FileStream::tell() const {
    HD_ASSERT(mFile);
    return static_cast<size_t>(ftell(mFile));
}

size_t FileStream::getSize() const {
    long curPos = ftell(mFile);
    fseek(mFile, 0, SEEK_END);
    long size = ftell(mFile);
    fseek(mFile, curPos, SEEK_SET);
    return static_cast<size_t>(size);
}

bool FileStream::seek(size_t pos) {
    HD_ASSERT(mFile);
    return fseek(mFile, static_cast<long>(pos), SEEK_SET) == 0;
}

bool FileStream::isEOF() const {
    return feof(mFile) != 0;
}

bool FileStream::isReadable() const {
    return mFile && mMode != FileMode::Write;
}

bool FileStream::isWritable() const {
    return mFile && mMode != FileMode::Read;
}

void FileStream::create(const std::string &path, FileMode mode) {
    destroy();
    std::string modeStr;
    if (mode == FileMode::Read) {
        modeStr = "rb";
    }
    else if (mode == FileMode::Write) {
        modeStr = "wb";
    }
    else if (mode == FileMode::ReadWrite) {
        modeStr = "rb+";
    }
    mFile = fopen(path.data(), modeStr.data());
    if (!mFile) {
        HD_LOG_ERROR("Failed to open file '%s' with flags '%s'", path.data(), modeStr.data());
    }
    mMode = mode;
    setName(path);
}

void FileStream::destroy() {
    if (mFile) {
        fclose(mFile);
        mFile = nullptr;
        mMode = FileMode::Read;
        setName("");
    }
}

}
