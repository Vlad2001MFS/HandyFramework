#include "hdStringStream.hpp"

namespace hd {

StringStream::StringStream::StringStream() {
    open("");
}

StringStream::StringStream(const std::string &str) {
    open(str);
}

StringStream::~StringStream() {
    close();
}

void StringStream::open(const std::string &str) {
    mData = str;
    mPos = 0;
}

void StringStream::close() {
    mData = "";
    mPos = 0;
}

char StringStream::read(const std::string &symbolsToSkip) {
    auto ch = look(0, symbolsToSkip);
    mPos++;
    return ch;
}

char StringStream::look(size_t offset, const std::string &symbolsToSkip) {
    if (!symbolsToSkip.empty()) {
        mSkip(symbolsToSkip);
    }
    if (mPos + offset < mData.length()) {
        return mData[mPos + offset];
    }
    else {
        return 0;
    }
}

bool StringStream::match(char ch, const std::string &symbolsToSkip) {
    if (look(0, symbolsToSkip) == ch) {
        read(symbolsToSkip);
        return true;
    }
    else {
        return false;
    }
}

void StringStream::mSkip(const std::string &symbolsToSkip) {
    while (StringUtils::containsSymbol(symbolsToSkip, mData[mPos], false)) {
        mPos++;
    }
}

}
