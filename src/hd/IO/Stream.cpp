#include "Stream.hpp"
#include "../Core/Log.hpp"

namespace hd {

std::string Stream::readLine(char separator) {
    std::string line;
    while (!isEOF()) {
        char ch;
        HD_ASSERT(read(&ch, sizeof(ch)) == sizeof(ch));
        if (ch == separator || ch == '\0') {
            break;
        }
        else {
            line += ch;
        }
    }
    return line;
}

std::string Stream::readAllText() {
    size_t size = getSize();
    std::string data(size, '\0');
    HD_ASSERT(read(data.data(), size) == size);
    return data;
}

void Stream::writeLine(const std::string &line) {
    HD_ASSERT(write(line.data(), line.size() + 1) == (line.size() + 1));
}

std::vector<uint8_t> Stream::readAllBuffer() {
    size_t bufSize = getSize();
    std::vector<uint8_t> buf;
    buf.resize(bufSize);
    if (read(buf.data(), bufSize) != bufSize) {
        HD_LOG_ERROR("Failed to load sound from stream '{}'", getName().data());
    }
    return buf;
}

void Stream::setName(const std::string &name) {
    mName = name;
}

const std::string &Stream::getName() const {
    return mName;
}

}
