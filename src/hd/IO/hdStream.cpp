#include "hdStream.hpp"
#include "../Core/hdLog.hpp"

namespace hd {

std::string Stream::readLine(char separator) {
    std::string line;
    while (!isEOF()) {
        char ch;
        read(&ch, sizeof(ch));
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
    if (read(data.data(), size) != size) {
        HD_LOG_ERROR("Failed to read all text from stream '%s'", getName().data());
    }
    return data;
}

size_t Stream::writeLine(const std::string &line) {
    return write(line.data(), line.size() + 1);
}

void Stream::setName(const std::string &name) {
    mName = name;
}

const std::string &Stream::getName() const {
    return mName;
}

}
