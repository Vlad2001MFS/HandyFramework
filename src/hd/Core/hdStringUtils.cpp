#include "hdStringUtils.hpp"
#include <cstdarg>
#include <codecvt>
#include <memory>

namespace hd {

#ifdef HD_PLATFORM_UNIX
size_t _vscprintf (const char *format, va_list pargs) {
    va_list argcopy;
    va_copy(argcopy, pargs);
    int ret = vsnprintf(nullptr, 0, format, argcopy);
    va_end(argcopy);
    return static_cast<size_t>(ret);
}
#endif

std::string StringUtils::format(const char * fmt, ...) {
    HD_ASSERT(fmt);
    va_list args;
    va_start(args, fmt);
    std::string buf(_vscprintf(fmt, args), '\0');
    vsprintf(buf.data(), fmt, args);
    va_end(args);
    return buf;
}

std::string StringUtils::formatArgs(const char * fmt, va_list args) {
    HD_ASSERT(fmt);
    std::string buf(_vscprintf(fmt, args), '\0');
    vsprintf(buf.data(), fmt, args);
    return buf.data();
}

int StringUtils::scan(const std::string &str, const char * fmt, ...) {
    HD_ASSERT(fmt);
    va_list ap;
    va_start(ap, fmt);
    int ret = vsscanf(str.data(), fmt, ap);
    va_end(ap);
    return ret;
}

std::smatch StringUtils::regexScan(const std::string &str, const char *fmt) {
    HD_ASSERT(fmt);
    std::regex rx(fmt);
    std::smatch match;
    std::regex_match(str, match, rx);
    return match;
}

std::vector<std::string> StringUtils::split(const std::string &str, const std::string &separators, bool saveSeparators) {
    std::vector<std::string> tokens;
    std::string token;
    for (const auto &it : str) {
        if (containsSymbol(separators, it, false)) {
            tokens.push_back(token);
            token.clear();
            if (saveSeparators) {
                token += it;
                tokens.push_back(token);
                token.clear();
            }
        }
        else {
            token += it;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string StringUtils::beforeFirst(const std::string &str, char separator) {
    return str.substr(0, str.find(separator));
}

std::string StringUtils::beforeFirst(const std::string &str, const std::string &separator) {
    return str.substr(0, str.find(separator));
}

std::string StringUtils::beforeLast(const std::string &str, char separator) {
    return str.substr(0, str.rfind(separator));
}

std::string StringUtils::beforeLast(const std::string &str, const std::string &separator) {
    return str.substr(0, str.rfind(separator));
}

std::string StringUtils::afterFirst(const std::string &str, char separator) {
    return str.substr(str.find(separator) + 1);
}

std::string StringUtils::afterFirst(const std::string &str, const std::string &separator) {
    return str.substr(str.find(separator) + 1);
}

std::string StringUtils::afterLast(const std::string &str, char separator) {
    return str.substr(str.rfind(separator) + 1);
}

std::string StringUtils::afterLast(const std::string &str, const std::string &separator) {
    return str.substr(str.rfind(separator) + 1);
}

std::string StringUtils::subStr(const std::string &str, char leftSeparator, char rightSeparator) {
    size_t offset = str.find(leftSeparator) + 1;
    size_t count = str.rfind(rightSeparator) - offset;
    return str.substr(offset, count);
}

std::string StringUtils::subStr(const std::string &str, const std::string &leftSeparator, const std::string &rightSeparator) {
    size_t offset = str.find(leftSeparator) + leftSeparator.size();
    size_t count = str.rfind(rightSeparator) - offset;
    return str.substr(offset, count);
}

std::string StringUtils::replace(const std::string& str, const std::string& from, const std::string& to) {
    size_t startPos = str.find(from);
    if (startPos == std::string::npos) {
        return str;
    }
    else {
        std::string result = str;
        result.replace(startPos, from.length(), to);
        return result;
    }
}

std::string StringUtils::removeSymbols(const std::string &str, const std::string &symbols, bool caseSensitive) {
    std::string result = str;
    for (const auto &ch : symbols) {
        if (caseSensitive) {
            result.erase(std::remove(result.begin(), result.end(), ch), result.end());
        }
        else {
            result.erase(std::remove(result.begin(), result.end(), lower(ch)), result.end());
            result.erase(std::remove(result.begin(), result.end(), upper(ch)), result.end());
        }
    }
    return result;
}

size_t StringUtils::symbolsCount(const std::string &str, const std::string &symbols, bool caseSensitive) {
    std::string s1 = caseSensitive ? str : lower(str);
    std::string s2 = caseSensitive ? symbols : lower(symbols);
    size_t count = 0;
    for (const auto &ch : s2) {
        count += symbolsCount(s1, ch, true);
    }
    return count;
}

size_t StringUtils::symbolsCount(const std::string &str, char symbol, bool caseSensitive) {
    std::string s1 = caseSensitive ? str : lower(str);
    char s2 = caseSensitive ? symbol : lower(symbol);
    size_t count = 0;
    for (const auto &ch : s1) {
        if (ch == s2) {
            count++;
        }
    }
    return count;
}

bool StringUtils::contains(const std::string & str, const std::string & findStr, bool caseSensitive) {
    if (caseSensitive)
        return str.find(findStr) != str.npos;
    else
        return lower(str).find(lower(findStr)) != std::string::npos;
}

bool StringUtils::containsSymbol(const std::string &str, char symbol, bool caseSensitive) {
    std::string s1 = caseSensitive ? str : lower(str);
    char ch = caseSensitive ? symbol : lower(symbol);
    for (const auto &it : s1) {
        if (it == ch) {
            return true;
        }
    }
    return false;
}

bool StringUtils::containsSymbol(const std::string &str, const std::string &symbolsList, bool caseSensitive) {
    std::string s1 = caseSensitive ? str : lower(str);
    std::string s2 = caseSensitive ? symbolsList : lower(symbolsList);
    for (const auto &ch : s2) {
        if (containsSymbol(s1, ch, true)) {
            return true;
        }
    }
    return false;
}

bool StringUtils::compare(const std::string & str1, const std::string & str2, bool caseSensetive) {
    if (caseSensetive) {
        return strcmp(str1.data(), str2.data()) == 0;
    }
    else {
#ifdef HD_PLATFORM_WIN
        return _stricmp(str1.data(), str2.data()) == 0;
#elif HD_PLATFORM_UNIX
        return strcasecmp(str1.data(), str2.data()) == 0;
#endif
    }
}

bool StringUtils::startsWith(const std::string &str, const std::string &substr, bool caseSensetive) {
    if (str.size() < substr.size()) {
        return false;
    }
    std::string s1 = caseSensetive ? str : lower(str);
    std::string s2 = caseSensetive ? substr : lower(substr);
    for (size_t i = 0; i < s2.size(); i++) {
        if (s1[i] != s2[i]) {
            return false;
        }
    }
    return true;
}

bool StringUtils::endsWith(const std::string &str, const std::string &substr, bool caseSensetive) {
    if (str.size() < substr.size()) {
        return false;
    }
    std::string s1 = caseSensetive ? str : lower(str);
    std::string s2 = caseSensetive ? substr : lower(substr);
    for (size_t i = 0; i < substr.size(); i++) {
        if (s1[s1.size() - 1 - i] != s2[s2.size() - 1 - i]) {
            return false;
        }
    }
    return true;
}

bool StringUtils::isDigit(char ch) {
    return std::isdigit(ch, std::locale());
}

bool StringUtils::isAlpha(char ch) {
    return std::isalpha(ch, std::locale());
}

bool StringUtils::isInt(const std::string &str) {
    for (const auto &it : str) {
        if (!isDigit(it)) {
            return false;
        }
    }
    return true;
}

bool StringUtils::isFloat(const std::string &str) {
    if (hd::StringUtils::symbolsCount(str, ".", false) != 1) {
        return false;
    }
    for (const auto &it : str) {
        if (!isDigit(it) && it != '.') {
            return false;
        }
    }
    return true;
}

bool StringUtils::isAlpha(const std::string &str) {
    for (const auto &it : str) {
        if (!isAlpha(it)) {
            return false;
        }
    }
    return true;
}

bool StringUtils::isAlphaDigit(const std::string &str) {
    for (const auto &it : str) {
        if (!isAlpha(it) && !isDigit(it)) {
            return false;
        }
    }
    return true;
}

std::wstring StringUtils::toWideString(const std::string & str) {
    size_t len = str.length() + 1;
    std::wstring buf(len, '\0');
    mbstowcs(buf.data(), str.data(), len);
    return buf;
}

std::string StringUtils::fromWideString(const std::wstring& str) {
    return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(str);
}

std::string StringUtils::utf8ToCp1251(const std::string &utf8str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> wconv;
    std::wstring wstr = wconv.from_bytes(utf8str);
    std::vector<char> buf(wstr.size());
    std::use_facet<std::ctype<wchar_t>>(std::locale(".1251")).narrow(wstr.data(), wstr.data() + wstr.size(), '?', buf.data());
    return std::string(buf.data(), buf.size());
}

std::string StringUtils::upper(const std::string & str) {
    std::string out(str.size(), '\0');
    std::transform(str.begin(), str.end(), out.begin(), toupper);
    return out;
}

char StringUtils::upper(char ch) {
    return static_cast<char>(toupper(ch));
}

std::string StringUtils::lower(const std::string & str) {
    std::string out(str.size(), '\0');
    std::transform(str.begin(), str.end(), out.begin(), tolower);
    return out;
}

char StringUtils::lower(char ch) {
    return static_cast<char>(tolower(ch));
}

uint64_t StringUtils::getHash(const std::string & str) {
    return std::hash<std::string>()(str);
}

bool StringUtils::toBool(const std::string & str) {
    return compare(str, "true", false);
}

int StringUtils::toInt(const std::string & str) {
    return std::stoi(str);
}

uint64_t StringUtils::toUint64(const std::string &str) {
    return std::stoull(str);
}

float StringUtils::toFloat(const std::string & str) {
    return std::stof(str);
}

double StringUtils::toDouble(const std::string & str) {
    return std::stod(str);
}

std::string StringUtils::fromBool(bool value) {
    return value ? "true" : "false";
}

std::string StringUtils::fromInt(int value) {
    return std::to_string(value);
}

std::string StringUtils::fromUint64(uint64_t value) {
    return std::to_string(value);
}

std::string StringUtils::fromFloat(float value) {
    return std::to_string(value);
}

std::string StringUtils::fromDouble(double value) {
    return std::to_string(value);
}

std::string StringUtils::fromVector(const std::vector<std::string> &v, const std::string &prefix, const std::string &postfix, const std::string &separator) {
    return fromVector(v, prefix, postfix, separator, [](const std::string &str) { return str; });
}

}
