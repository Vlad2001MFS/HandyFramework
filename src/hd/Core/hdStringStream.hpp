#pragma once
#include "hdCommon.hpp"
#include "hdStringUtils.hpp"

namespace hd {

class StringStream
{
public:
    StringStream();
    StringStream(const std::string &str);
    ~StringStream();

    void open(const std::string &str);
    void close();

    char read(const std::string &symbolsToSkip);
    char look(size_t offset, const std::string &symbolsToSkip);
    bool match(char ch, const std::string &symbolsToSkip);
    bool isEOF() const { return mPos >= mData.length(); }
    const std::string &getData() const { return mData; }
    size_t getPosition() const { return mPos; }

private:
    void mSkip(const std::string &symbolsToSkip);

    std::string mData;
    size_t mPos;
};

}
