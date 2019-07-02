#include "hdConfig.hpp"
#include "hdFileStream.hpp"
#include "../Core/hdStringStream.hpp"

namespace hd {

enum class TokenType {
    BraceOpen,
    BraceClose,
    ParenOpen,
    ParenClose,
    BracketOpen,
    BracketClose,
    Equal,
    Comma,
    Identifier,
    Null,
    Bool,
    Int,
    Float,
    String
};

struct Token {
    Token(TokenType type, const std::string &value) : type(type), value(value) {}

    TokenType type;
    std::string value;
};

class Lexer {
public:
    Lexer() {}
    ~Lexer() {}

    std::vector<Token> parse(const std::string &text) {
        std::vector<std::string> lexems = hd::StringUtils::split(hd::StringUtils::removeSymbols(text, mIgnoreSymbols, false), mSingleSymbols + mSeparators, true);
        lexems.erase(std::remove_if(lexems.begin(), lexems.end(), [](const std::string &str) {
            return str.empty() || str == " " || str == "\t";
        }), lexems.end());

        std::vector<Token> tokens;
        for (const auto &it : lexems) {
            if (it == "{") {
                tokens.emplace_back(TokenType::BraceOpen, it);
            }
            else if (it == "}") {
                tokens.emplace_back(TokenType::BraceClose, it);
            }
            else if (it == "(") {
                tokens.emplace_back(TokenType::ParenOpen, it);
            }
            else if (it == ")") {
                tokens.emplace_back(TokenType::ParenClose, it);
            }
            else if (it == "[") {
                tokens.emplace_back(TokenType::BracketOpen, it);
            }
            else if (it == "]") {
                tokens.emplace_back(TokenType::BracketClose, it);
            }
            else if (it == "=") {
                tokens.emplace_back(TokenType::Equal, it);
            }
            else if (it == ",") {
                tokens.emplace_back(TokenType::Comma, it);
            }
            else if (it == "null") {
                tokens.emplace_back(TokenType::Null, it);
            }
            else if (it == "true" || it == "false") {
                tokens.emplace_back(TokenType::Bool, it);
            }
            else if (hd::StringUtils::isInt(it)) {
                tokens.emplace_back(TokenType::Int, it);
            }
            else if (hd::StringUtils::isFloat(it)) {
                tokens.emplace_back(TokenType::Float, it);
            }
            else if (it.front() == '"' && it.back() == '"') {
                tokens.emplace_back(TokenType::String, it.substr(1, it.size() - 2));
            }
            else if (hd::StringUtils::isAlpha(it.front()) && hd::StringUtils::isAlphaDigit(it)) {
                tokens.emplace_back(TokenType::Identifier, it);
            }
            else {
                HD_LOG_ERROR("Unknown lexem '%s'", it.data());
            }
        }
        return tokens;
    }

private:
    const std::string mIgnoreSymbols = "\r\n";
    const std::string mSingleSymbols = "{}()[]=,";
    const std::string mSeparators = " \t";
};

class Parser {
public:
    Parser(const std::string &name) : mName(name) {}
    ~Parser() {}

    void parse(const std::vector<Token> &tokens, ConfigValue &root) {
        mTokens = tokens;
        mIt = mTokens.begin();
        root = ConfigValue(ConfigValueType::Table);
        root.mName = mName;
        const Token &token = mRead();
        if (token.type == TokenType::BraceOpen) {
            mParseTable(root);
        }
        else if (token.type == TokenType::BracketOpen) {
            mParseArray(root);
        }
        else {
            HD_LOG_ERROR("Unknown token at the begin of config '%s'", mName.data());
        }
    }

private:
    const Token &mLook(int offset) {
        return *(mIt + offset);
    }

    const Token &mRead() {
        const Token &token = *mIt;
        mIt++;
        return token;
    }

    bool mMatch(TokenType type) {
        if (mLook(0).type == type) {
            mRead();
            return true;
        }
        else {
            return false;
        }
    }

    void mParseTable(ConfigValue &root) {
        while (!mMatch(TokenType::BraceClose)) {
            mParseSlot(root);
            if (mLook(0).type != TokenType::BraceClose) {
                mMatch(TokenType::Comma);
            }
        }
    }

    void mParseSlot(ConfigValue &root) {
        const Token &id = mRead();
        mMatch(TokenType::Equal);
        ConfigValue value;
        mParseValue(value);
        root.addSlot(id.value, value);
    }

    void mParseValue(ConfigValue &root) {
        const Token &value = mRead();
        if (value.type == TokenType::Null) {
            root = ConfigValue();
            root.mName = mName;
        }
        else if (value.type == TokenType::Bool) {
            root = ConfigValue(value.value == "true");
            root.mName = mName;
        }
        else if (value.type == TokenType::Int) {
            root = ConfigValue(hd::StringUtils::toInt(value.value));
            root.mName = mName;
        }
        else if (value.type == TokenType::Float) {
            root = ConfigValue(hd::StringUtils::toFloat(value.value));
            root.mName = mName;
        }
        else if (value.type == TokenType::String) {
            root = ConfigValue(value.value);
            root.mName = mName;
        }
        else if (value.type == TokenType::BracketOpen) {
            root = ConfigValue(ConfigValueType::Array);
            root.mName = mName;
            mParseArray(root);
        }
        else if (value.type == TokenType::BraceOpen) {
            root = ConfigValue(ConfigValueType::Table);
            root.mName = mName;
            mParseTable(root);
        }
    }

    void mParseArray(ConfigValue &root) {
        while (!mMatch(TokenType::BracketClose)) {
            ConfigValue value;
            mParseValue(value);
            root.addElement(value);
            if (mLook(0).type != TokenType::BracketClose) {
                mMatch(TokenType::Comma);
            }
        }
    }

    std::string mName;
    std::vector<Token> mTokens;
    std::vector<Token>::iterator mIt;
};

ConfigValue::ConfigValue(ConfigValueType type) : mType(type) {
    if (type == ConfigValueType::Bool) {
        ConfigValue(false);
    }
    else if (type == ConfigValueType::Int) {
        ConfigValue(0);
    }
    else if (type == ConfigValueType::Float) {
        ConfigValue(0.0f);
    }
    else if (type == ConfigValueType::String) {
        ConfigValue(std::string(""));
    }
    else if (type == ConfigValueType::Array) {
        ConfigValue(std::vector<ConfigValue>({}));
    }
    else if (type == ConfigValueType::Table) {
        ConfigValue(std::map<std::string, ConfigValue>({}));
    }
}

ConfigValue::ConfigValue(bool value) {
    mType = ConfigValueType::Bool;
    mBool = value;
}

ConfigValue::ConfigValue(int value) {
    mType = ConfigValueType::Int;
    mInt = value;
}

ConfigValue::ConfigValue(float value) {
    mType = ConfigValueType::Float;
    mFloat = value;
}

ConfigValue::ConfigValue(const std::string &value) {
    mType = ConfigValueType::String;
    mString = value;
}

ConfigValue::ConfigValue(const std::vector<ConfigValue> &value) {
    mType = ConfigValueType::Array;
    mArray = value;
}

ConfigValue::ConfigValue(const std::map<std::string, ConfigValue> &value) {
    mType = ConfigValueType::Table;
    mTable = value;
}

ConfigValue::~ConfigValue() { }

bool ConfigValue::toBool() const {
    if (mType != ConfigValueType::Bool) {
        HD_LOG_ERROR("Failed cast to bool value in config '%s'", mName.data());
    }
    return mBool;
}

int ConfigValue::toInt() const {
    if (mType != ConfigValueType::Int) {
        HD_LOG_ERROR("Failed cast to int value in config '%s'", mName.data());
    }
    return mInt;
}

float ConfigValue::toFloat() const {
    if (mType != ConfigValueType::Float) {
        HD_LOG_ERROR("Failed cast to float value in config '%s'", mName.data());
    }
    return mFloat;
}

const std::string &ConfigValue::toString() const {
    if (mType != ConfigValueType::String) {
        HD_LOG_ERROR("Failed cast to string value in config '%s'", mName.data());
    }
    return mString;
}

const std::vector<ConfigValue> &ConfigValue::toArray() const {
    if (mType != ConfigValueType::Array) {
        HD_LOG_ERROR("Failed cast to array value in config '%s'", mName.data());
    }
    return mArray;
}

const std::map<std::string, ConfigValue> &ConfigValue::toTable() const {
    if (mType != ConfigValueType::Table) {
        HD_LOG_ERROR("Failed cast to table value in config '%s'", mName.data());
    }
    return mTable;
}

/*glm::vec2 ConfigValue::toVector2() const {
    auto array = toArray();
    if (mType != ConfigValueType::Array || array.size() != 2 || array.front().mType != ConfigValueType::Float) {
        HD_LOG_ERROR("Failed cast to Vector2 value in config '%s'", mName.data());
    }
    return glm::vec2(array[0].toFloat(), array[1].toFloat());
}

glm::ivec2 ConfigValue::toIntVector2() const {
    auto array = toArray();
    if (mType != ConfigValueType::Array || array.size() != 2 || array.front().mType != ConfigValueType::Int) {
        HD_LOG_ERROR("Failed cast to IntVector2 value in config '%s'", mName.data());
    }
    return glm::ivec2(array[0].toInt(), array[1].toInt());
}

glm::vec3 ConfigValue::toVector3() const {
    auto array = toArray();
    if (mType != ConfigValueType::Array || array.size() != 3 || array.front().mType != ConfigValueType::Float) {
        HD_LOG_ERROR("Failed cast to Vector3 value in config '%s'", mName.data());
    }
    return glm::vec3(array[0].toFloat(), array[1].toFloat(), array[2].toFloat());
}

glm::ivec3 ConfigValue::toIntVector3() const {
    auto array = toArray();
    if (mType != ConfigValueType::Array || array.size() != 3 || array.front().mType != ConfigValueType::Int) {
        HD_LOG_ERROR("Failed cast to IntVector3 value in config '%s'", mName.data());
    }
    return glm::ivec3(array[0].toInt(), array[1].toInt(), array[2].toInt());
}

glm::vec4 ConfigValue::toVector4() const {
    auto array = toArray();
    if (mType != ConfigValueType::Array || array.size() != 4 || array.front().mType != ConfigValueType::Float) {
        HD_LOG_ERROR("Failed cast to Vector4 value in config '%s'", mName.data());
    }
    return glm::vec4(array[0].toFloat(), array[1].toFloat(), array[2].toFloat(), array[3].toFloat());
}

glm::ivec4 ConfigValue::toIntVector4() const {
    auto array = toArray();
    if (mType != ConfigValueType::Array || array.size() != 4 || array.front().mType != ConfigValueType::Int) {
        HD_LOG_ERROR("Failed cast to IntVector4 value in config '%s'", mName.data());
    }
    return glm::ivec4(array[0].toInt(), array[1].toInt(), array[2].toInt(), array[3].toInt());
}*/


void ConfigValue::addSlot(const std::string &name, const ConfigValue &value) {
    if (mType != ConfigValueType::Table) {
        HD_LOG_ERROR("'Try to add slot to non-table value in config '%s'", mName.data());
    }
    mTable[name] = value;
}

void ConfigValue::addElement(const ConfigValue &value) {
    if (mType != ConfigValueType::Array) {
        HD_LOG_ERROR("'Try to add element to non-array value in config '%s'", mName.data());
    }
    if (!mArray.empty() && value.mType != mArray.front().mType) {
        HD_LOG_ERROR("'Try to add element with type which not equal array-frong value type to array value in config '%s'", mName.data());
    }
    mArray.push_back(value);
}

const ConfigValue &ConfigValue::operator[](size_t idx) const {
    if (mType != ConfigValueType::Array) {
        HD_LOG_ERROR("'Try to index '%d' non-array value in config '%s'", idx, mName.data());
    }
    if (idx >= toArray().size()) {
        HD_LOG_ERROR("Failed to get element '%d' from array in config '%s'", idx, mName.data());
    }
    return toArray().at(idx);
}

const ConfigValue &ConfigValue::operator[](const std::string &name) const {
    if (mType != ConfigValueType::Table) {
        HD_LOG_ERROR("Try to index non-table value in config '%s'", mName.data());
    }
    if (!toTable().count(name)) {
        HD_LOG_ERROR("Failed to get slot '%s' from table in config '%s'", name.data(), mName.data());
    }
    return toTable().at(name);
}

Config::Config() {
}

Config::Config(StreamReader &stream) {
    create(stream);
}

Config::Config(const std::string &filename) {
    create(filename);
}

Config::~Config() {
    destroy();
}

void Config::create(StreamReader &stream) {
    mName = stream.getName();
    uint32_t size = stream.getSize();
    std::string text(size, '\0');
    stream.read(text.data(), size);

    Lexer lexer;
    std::vector<Token> tokens = lexer.parse(text);
    Parser parser(mName);
    parser.parse(tokens, *this);
}

void Config::create(const std::string &filename) {
    hd::FileReader fs(filename);
    create(fs);
}

void Config::destroy() {  }

}
