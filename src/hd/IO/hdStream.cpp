#include "hdStream.hpp"

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
    std::vector<std::string> data;
    while (!isEOF()) {
        data.push_back(readLine());
    }
    return data;
}

std::string StreamReader::readAllText() {
    std::string data;
    auto size = getSize();
    data.resize(size);
    HD_ASSERT(read(data.data(), sizeof(char)*size) == sizeof(char)*size);
    return data;
}

StreamWriter::~StreamWriter() {}

}
