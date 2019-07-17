#include "hdStream.hpp"
#include "../Core/hdStringUtils.hpp"

namespace hd {

Stream::Stream() {}
Stream::~Stream() {}

StreamReader::~StreamReader() {}

std::string StreamReader::readLine(char separator) {
    std::string line;
    while (!isEOF()) {
        char ch;
        read(&ch, sizeof(ch));
        if (ch == separator) {
            break;
        }
        else {
            line += ch;
        }
    }
    return line;
}

std::vector<std::string> StreamReader::readAllLines() {
    return StringUtils::split(readAllText(), "\n", false);
}

std::string StreamReader::readAllText() {
    uint32_t size = getSize();
    std::string data(size, '\0');
    if (read(data.data(), size) != size) {
        HD_LOG_ERROR("Failed to read all text from stream '%s'", getName().data());
    }
    return data;
}

StreamWriter::~StreamWriter() {}

size_t StreamWriter::writeString(const std::string &data) {
    return write(data.data(), sizeof(char)*data.size());
}

}
