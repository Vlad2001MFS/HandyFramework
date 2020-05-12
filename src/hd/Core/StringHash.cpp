#include "StringHash.hpp"
#include "Log.hpp"
#include "Common.hpp"
#include <unordered_map>

namespace hd {

static auto &getStringHashDB() {
    static std::unordered_map<uint64_t, std::string> db;
    return db;
}

StringHash::StringHash() {
    mValue = 0;
}

StringHash::StringHash(const StringHash &rhs) {
    mValue = rhs.mValue;
}

StringHash::StringHash(uint64_t value) {
    mValue = value;
}

StringHash::StringHash(const std::string &str) {
    mValue = std::hash<std::string>()(str);
    if (!getStringHashDB().count(mValue)) {
        getStringHashDB().insert(std::make_pair(mValue, str));
    }
}

bool StringHash::operator==(const StringHash& rhs) const {
    return mValue == rhs.mValue;
}

bool StringHash::operator!=(const StringHash& rhs) const {
    return mValue != rhs.mValue;
}

bool StringHash::operator<(const StringHash& rhs) const {
    return mValue < rhs.mValue;
}

bool StringHash::operator>(const StringHash& rhs) const {
    return mValue > rhs.mValue;
}

StringHash::operator bool() const {
    return mValue != 0;
}

uint64_t StringHash::getHash() const {
    return mValue;
}

const std::string &StringHash::getString() const {
    auto it = getStringHashDB().find(mValue);
    if (it != getStringHashDB().end()) {
        return it->second;
    }
    else {
        HD_LOG_WARNING("Failed to get string from hash '{}'", mValue);
        static std::string empty;
        return empty;
    }
}

}
