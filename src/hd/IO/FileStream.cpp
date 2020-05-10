#include "FileStream.hpp"
#include "../Core/Log.hpp"
#include <filesystem>

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
    HD_ASSERT(isReadable());
    return fread(data, 1, size, mFile);
}

size_t FileStream::write(const void *data, size_t size) {
    HD_ASSERT(mFile);
    HD_ASSERT(isWritable());
    return fwrite(data, 1, size, mFile);
}

size_t FileStream::tell() const {
    HD_ASSERT(mFile);
    return static_cast<size_t>(ftell(mFile));
}

size_t FileStream::getSize() const {
    return std::filesystem::file_size(getName());
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
    mMode = mode;
    setName(path);

    std::string modeStr;
    if (mode == FileMode::Read) {
        modeStr = "rb";
    }
    else if (mode == FileMode::Write) {
        modeStr = "wb";
    }
    else if (mode == FileMode::ReadWrite) {
        modeStr = "wb+";
    }
    mFile = fopen(path.data(), modeStr.data());
    HD_ASSERT(mFile);
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
