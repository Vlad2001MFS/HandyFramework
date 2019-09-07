#include "hdStringUtils.hpp"
#include "hdLog.hpp"
#include <algorithm>
#include <locale>
#include <codecvt>
#include <cstdarg>
#include <cstring>

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

std::string StringUtils::format(const char *fmt, ...) {
    HD_ASSERT(fmt);
    va_list args;
    va_start(args, fmt);
    std::string str = formatArgs(fmt, args);
    va_end(args);
    return str;
}

std::string StringUtils::formatArgs(const char *fmt, va_list args) {
    HD_ASSERT(fmt);
    int len = _vscprintf(fmt, args);
    std::string buf(static_cast<size_t>(len), '\0');
    vsprintf(buf.data(), fmt, args);
    return buf.data();
}

int StringUtils::scan(const std::string &str, const char *fmt, ...) {
    HD_ASSERT(fmt);
    va_list args;
    va_start(args, fmt);
    int ret = scanArgs(str, fmt, args);
    va_end(args);
    return ret;
}

int StringUtils::scanArgs(const std::string &str, const char *fmt, va_list args) {
    HD_ASSERT(fmt);
    return vsscanf(str.data(), fmt, args);
}

std::vector<std::string> StringUtils::split(const std::string &str, const std::string &separatorsList, bool saveSeparators) {
    std::vector<std::string> tokens;
    std::string token;
    for (const auto &it : str) {
        if (containsSymbol(separatorsList, it, false)) {
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

std::string StringUtils::beforeLast(const std::string &str, char separator) {
    return str.substr(0, str.rfind(separator));
}

std::string StringUtils::afterFirst(const std::string &str, char separator) {
    return str.substr(str.find(separator) + 1);
}

std::string StringUtils::afterLast(const std::string &str, char separator) {
    return str.substr(str.rfind(separator) + 1);
}

std::string StringUtils::subStr(const std::string &str, char leftSeparator, char rightSeparator) {
    size_t offset = str.find(leftSeparator) + 1;
    size_t count = str.rfind(rightSeparator) - offset;
    return str.substr(offset, count);
}

std::string StringUtils::replace(const std::string &str, const std::string &from, const std::string &to) {
    size_t startPos = str.find(from);
    if (startPos == std::string::npos) {
        return str;
    }
    std::string result = str;
    result.replace(startPos, from.length(), to);
    return result;
}

std::string StringUtils::removeSymbols(const std::string &str, const std::string &symbolsList, bool caseSensitive) {
    std::string result = str;
    for (const auto &ch : symbolsList) {
        if (caseSensitive) {
            result.erase(std::remove(result.begin(), result.end(), ch), result.end());
        }
        else {
            result.erase(std::remove(result.begin(), result.end(), toLower(ch)), result.end());
            result.erase(std::remove(result.begin(), result.end(), toUpper(ch)), result.end());
        }
    }
    return result;
}

size_t StringUtils::symbolsCount(const std::string &str, const std::string &symbolsList, bool caseSensitive) {
    std::string s1 = caseSensitive ? str : toLower(str);
    std::string s2 = caseSensitive ? symbolsList : toLower(symbolsList);
    size_t count = 0;
    for (const auto &ch : s2) {
        count += symbolsCount(s1, ch, true);
    }
    return count;
}

size_t StringUtils::symbolsCount(const std::string &str, char symbol, bool caseSensitive) {
    std::string s1 = caseSensitive ? str : toLower(str);
    char s2 = caseSensitive ? symbol : toLower(symbol);
    size_t count = 0;
    for (const auto &ch : s1) {
        if (ch == s2) {
            count++;
        }
    }
    return count;
}

bool StringUtils::contains(const std::string &str, const std::string &findStr, bool caseSensitive) {
    if (caseSensitive)
        return str.find(findStr) != str.npos;
    else
        return toLower(str).find(toLower(findStr)) != std::string::npos;
}

bool StringUtils::containsSymbol(const std::string &str, char symbol, bool caseSensitive) {
    std::string s1 = caseSensitive ? str : toLower(str);
    char ch = caseSensitive ? symbol : toLower(symbol);
    for (const auto &it : s1) {
        if (it == ch) {
            return true;
        }
    }
    return false;
}

bool StringUtils::containsSymbols(const std::string &str, const std::string &symbolsList, bool caseSensitive) {
    std::string s1 = caseSensitive ? str : toLower(str);
    std::string s2 = caseSensitive ? symbolsList : toLower(symbolsList);
    for (const auto &ch : s2) {
        if (containsSymbol(s1, ch, true)) {
            return true;
        }
    }
    return false;
}

bool StringUtils::compare(const std::string &str1, const std::string &str2, bool caseSensetive) {
    if (caseSensetive) {
        return strcmp(str1.data(), str2.data()) == 0;
    }
    else {
#ifdef HD_PLATFORM_WIN
        return _stricmp(str1.data(), str2.data()) == 0;
#else
        return strcasecmp(str1.data(), str2.data()) == 0;
#endif
    }
}

bool StringUtils::startsWith(const std::string &str, const std::string &substr, bool caseSensetive) {
    if (str.size() < substr.size()) {
        return false;
    }
    std::string s1 = caseSensetive ? str : toLower(str);
    std::string s2 = caseSensetive ? substr : toLower(substr);
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
    std::string s1 = caseSensetive ? str : toLower(str);
    std::string s2 = caseSensetive ? substr : toLower(substr);
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
    if (symbolsCount(str, ".", false) != 1) {
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

std::wstring StringUtils::toWideString(const std::string &str) {
    size_t len = str.length() + 1;
    std::wstring buf(len, '\0');
    mbstowcs(buf.data(), str.data(), len);
    return buf;
}

std::string StringUtils::fromWideString(const std::wstring &str) {
    return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(str);
}

std::string StringUtils::utf8ToCp1251(const std::string &utf8str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> wconv;
    std::wstring wstr = wconv.from_bytes(utf8str);
    std::vector<char> buf(wstr.size());
    std::use_facet<std::ctype<wchar_t>>(std::locale(".1251"))
        .narrow(wstr.data(), wstr.data() + wstr.size(), '?', buf.data());
    return std::string(buf.data(), buf.size());
}

std::string StringUtils::toUpper(const std::string &str) {
    std::string out(str.size(), '\0');
    std::transform(str.begin(), str.end(), out.begin(), toupper);
    return out;
}

char StringUtils::toUpper(char ch) {
    return std::toupper(ch, std::locale());
}

std::string StringUtils::toLower(const std::string &str) {
    std::string out(str.size(), '\0');
    std::transform(str.begin(), str.end(), out.begin(), tolower);
    return out;
}

char StringUtils::toLower(char ch) {
    return std::tolower(ch, std::locale());
}

bool StringUtils::toBool(const std::string &str) {
    return compare(str, "true", false);
}

int StringUtils::toInt(const std::string &str) {
    return std::stoi(str);
}

uint64_t StringUtils::toUint64(const std::string &str) {
    return std::stoull(str);
}

float StringUtils::toFloat(const std::string &str) {
    return std::stof(str);
}

double StringUtils::toDouble(const std::string &str) {
    return std::stod(str);
}

std::string StringUtils::fromBool(bool value) {
    return value ? "true" : "false";
}

}
