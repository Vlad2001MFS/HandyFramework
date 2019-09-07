#pragma once
#include "../Core/hdMacros.hpp"
#include <string>

namespace hd {

class FileSystem {
    HD_STATIC_CLASS(FileSystem);
public:
    static std::string replaceExtension(const std::string &str, const std::string &ext);
};

}
