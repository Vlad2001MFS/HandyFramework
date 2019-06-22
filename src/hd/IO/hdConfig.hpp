#pragma once
#include "hdStream.hpp"
#include <vector>
#include <map>

namespace hd {

enum class ConfigValueType {
    Null,
    Bool,
    Int,
    Float,
    String,
    Array,
    Table
};

class ConfigValue {
    friend class Parser;
public:
    explicit ConfigValue(ConfigValueType type = ConfigValueType::Null);
    explicit ConfigValue(bool value);
    explicit ConfigValue(int value);
    explicit ConfigValue(float value);
    explicit ConfigValue(const std::string &value);
    explicit ConfigValue(const std::vector<ConfigValue> &value);
    explicit ConfigValue(const std::map<std::string, ConfigValue> &value);
    virtual ~ConfigValue();

    bool toBool() const;
    int toInt() const;
    float toFloat() const;
    const std::string &toString() const;
    const std::vector<ConfigValue> &toArray() const;
    const std::map<std::string, ConfigValue> &toTable() const;
    /*glm::vec2 toVector2() const;
    glm::ivec2 toIntVector2() const;
    glm::vec3 toVector3() const;
    glm::ivec3 toIntVector3() const;
    glm::vec4 toVector4() const;
    glm::ivec4 toIntVector4() const;*/

    void addSlot(const std::string &name, const ConfigValue &value);
    void addElement(const ConfigValue &value);

    const ConfigValue &operator[](size_t idx) const;
    const ConfigValue &operator[](const std::string &name) const;

protected:
    std::string mName;
    ConfigValueType mType;
    bool mBool;
    int mInt;
    float mFloat;
    std::string mString;
    std::vector<ConfigValue> mArray;
    std::map<std::string, ConfigValue> mTable;
};

class Config : public ConfigValue {
public:
    Config();
    explicit Config(StreamReader &stream);
    explicit Config(const std::string &filename);
    virtual ~Config();

    void create(StreamReader &stream);
    void create(const std::string &filename);

    void destroy();
};

}
