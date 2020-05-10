#pragma once
#include "Common.hpp"
#include <string>
#include <regex>
#include <unordered_map>

namespace hd {

class StringUtils : public StaticClass {
public:
    static std::vector<std::string> split(const std::string &str, const std::string &separatorsList, bool saveSeparators);
    static std::string beforeFirst(const std::string &str, char separator);
    static std::string beforeLast(const std::string &str, char separator);
    static std::string afterFirst(const std::string &str, char separator);
    static std::string afterLast(const std::string &str, char separator);
    static std::string subStr(const std::string &str, char leftSeparator, char rightSeparator);
    static std::string replace(const std::string &str, const std::string &from, const std::string &to);
    static std::string removeSymbols(const std::string &str, const std::string &symbolsList, bool caseSensitive);
    static size_t symbolsCount(const std::string &str, const std::string &symbolsList, bool caseSensitive);
    static size_t symbolsCount(const std::string &str, char symbol, bool caseSensitive);
    static bool contains(const std::string &str, const std::string &findStr, bool caseSensitive);
    static bool containsSymbols(const std::string &str, const std::string &symbolsList, bool caseSensitive);
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
    static std::string toUpper(const std::string &str);
    static char toUpper(char ch);
    static std::string toLower(const std::string &str);
    static char toLower(char ch);
    static bool toBool(const std::string &str);
    static int toInt(const std::string &str);
    static uint64_t toUint64(const std::string &str);
    static float toFloat(const std::string &str);
    static double toDouble(const std::string &str);
    static std::string fromBool(bool value);
    static const std::string &getEmpty();

    template<typename T, typename F>
    static std::string unite(const T &v, const std::string &prefix, const std::string &postfix, const std::string &separator, F getStringFunc) {
        std::string str;
        for (auto it = v.begin(); it != v.end(); it++) {
            str += prefix + getStringFunc(*it) + postfix;
            if (it != (v.end() - 1)) {
                str += separator;
            }
        }
        return str;
    }

    template<typename T>
    static std::string unite(const T &v, const std::string &prefix, const std::string &postfix, const std::string &separator) {
        std::string str;
        for (auto it = v.begin(); it != v.end(); it++) {
            str += prefix + *it + postfix;
            if (it != (v.end() - 1)) {
                str += separator;
            }
        }
        return str;
    }
};

}
