#pragma once
#include "hdStream.hpp"
#include "../../3rd/include/glm/glm.hpp"
#include <vector>

namespace hd {

enum class ImageFormat {
    None,
    Grey,
    GreyAlpha,
    RGB,
    RGBA
};

class Image final {
public:
    Image();
    Image(const void *data, const glm::ivec2 &size, ImageFormat fmt);
    Image(Stream &stream, ImageFormat requiredFmt = ImageFormat::None);
    Image(const std::string &path, ImageFormat requiredFmt = ImageFormat::None);
    ~Image();

    void create(const void *data, const glm::ivec2 &size, ImageFormat fmt);
    void create(Stream &stream, ImageFormat requiredFmt = ImageFormat::None);
    void create(const std::string &path, ImageFormat requiredFmt = ImageFormat::None);
    void destroy();

    const void *getData() const;
    void *getData();
    const glm::ivec2 &getSize() const;
    ImageFormat getFormat() const;
    const std::string &getPath() const;

private:
    std::vector<uint8_t> mData;
    glm::ivec2 mSize;
    ImageFormat mFmt;
    std::string mPath;
};

}
