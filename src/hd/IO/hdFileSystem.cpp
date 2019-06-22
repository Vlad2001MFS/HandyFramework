#include "hdFileSystem.hpp"
#include "../Core/hdStringUtils.hpp"

namespace hd {

std::string hd::FileSystem::replaceExtension(const std::string &str, const std::string &ext) {
    return StringUtils::beforeLast(str, '.') + "." + ext;
}

}
