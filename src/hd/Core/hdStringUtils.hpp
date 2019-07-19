#pragma once
#include "hdCommon.hpp"
#include <string>
#include <regex>

namespace hd {

class StringUtils {
	HD_STATIC_CLASS(StringUtils)
public:
    static std::string format(const char * fmt, ...);
    static std::string formatArgs(const char * fmt, va_list args);
    static int scan(const std::string &str, const char * fmt, ...);
    static std::smatch regexScan(const std::string &str, const char *fmt);
    static std::vector<std::string> split(const std::string &str, const std::string &separators, bool saveSeparators);
    static std::string beforeFirst(const std::string &str, char separator);
    static std::string beforeFirst(const std::string &str, const std::string &separator);
    static std::string beforeLast(const std::string &str, char separator);
    static std::string beforeLast(const std::string &str, const std::string &separator);
    static std::string afterFirst(const std::string &str, char separator);
    static std::string afterFirst(const std::string &str, const std::string &separator);
    static std::string afterLast(const std::string &str, char separator);
    static std::string afterLast(const std::string &str, const std::string &separator);
    static std::string subStr(const std::string &str, char leftSeparator, char rightSeparator);
    static std::string subStr(const std::string &str, const std::string &leftSeparator, const std::string &rightSeparator);
    static std::string replace(const std::string &str, const std::string &from, const std::string &to);
    static std::string removeSymbols(const std::string &str, const std::string &symbols, bool caseSensitive);
    static size_t symbolsCount(const std::string &str, const std::string &symbols, bool caseSensitive);
    static size_t symbolsCount(const std::string &str, char symbol, bool caseSensitive);
    static bool contains(const std::string &str, const std::string &findStr, bool caseSensitive);
    static bool containsSymbol(const std::string &str, const std::string &symbolsList, bool caseSensitive);
    static bool containsSymbol(const std::string &str, char symbol, bool caseSensitive);
    static bool compare(const std::string &str1, const std::string &str2, bool caseSensetive);
    static bool startsWith(const std::string &str, const std::string &substr, bool caseSensetive);
    static bool endsWith(const std::string &str, const std::string &substr, bool caseSensetive);
    static bool isDigit(char ch);
    static bool isAlpha(char ch);
    static bool isInt(const std::string &str);
    static bool isFloat(const std::string &str);
    static bool isAlpha(const std::string &str);
    static bool isAlphaDigit(const std::string &str);
    static std::wstring toWideString(const std::string &str);
    static std::string fromWideString(const std::wstring &str);
    static std::string utf8ToCp1251(const std::string &utf8str);
    static std::string upper(const std::string &str);
    static char upper(char ch);
    static std::string lower(const std::string &str);
    static char lower(char ch);
    static uint64_t getHash(const std::string &str);
    static bool toBool(const std::string &str);
    static int toInt(const std::string &str);
    static uint64_t toUint64(const std::string &str);
    static float toFloat(const std::string &str);
    static double toDouble(const std::string &str);
    static std::string fromBool(bool value);
    static std::string fromInt(int value);
    static std::string fromUint64(uint64_t value);
    static std::string fromFloat(float value);
    static std::string fromDouble(double value);
    static std::string fromVector(const std::vector<std::string> &v, const std::string &prefix, const std::string &postfix, const std::string &separator);

    template<typename T, typename F>
    static std::string fromVector(const std::vector<T> &v, const std::string &prefix, const std::string &postfix, const std::string &separator, F getStringFunc) {
        std::string str;
        for (size_t i = 0; i < v.size(); i++) {
            str += prefix + getStringFunc(v[i]) + postfix;
            if (i < (v.size() - 1)) {
                str += separator;
            }
        }
        return str;
    }
};

}
