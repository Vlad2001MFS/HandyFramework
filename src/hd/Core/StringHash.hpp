#pragma once
#include <string>

namespace hd {

class StringHash {
public:
    StringHash();
    StringHash(const StringHash& rhs);
    explicit StringHash(uint64_t value);
    explicit StringHash(const std::string &str);

    bool operator ==(const StringHash& rhs) const;
    bool operator !=(const StringHash& rhs) const;
    bool operator <(const StringHash& rhs) const;
    bool operator >(const StringHash& rhs) const;
    explicit operator bool() const;

    uint64_t getHash() const;
    const std::string &getString() const;

private:
    uint64_t mValue;
};

}

namespace std {

template<>
struct hash<hd::StringHash> {
    size_t operator()(const hd::StringHash &s) const {
        static_assert(std::is_same<size_t, uint64_t>::value, "Cannot convert StringHash to std::hash because size_t isn't same to uint64_t");
        return s.getHash();
    }
};

}