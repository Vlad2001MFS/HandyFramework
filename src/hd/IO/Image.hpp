#pragma once
#include "Stream.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace hd {

enum class ImageFormat {
    None,
    Grey,
    GreyAlpha,
    RGB,
    RGBA
};

class Image {
public:
    Image();
    Image(const void *data, const glm::ivec2 &size, ImageFormat fmt);
    explicit Image(Stream &stream, ImageFormat requiredFmt = ImageFormat::None, bool flipVertically = false);
    explicit Image(const std::string &path, ImageFormat requiredFmt = ImageFormat::None, bool flipVertically = false);
    ~Image();

    void create(const void *data, const glm::ivec2 &size, ImageFormat fmt);
    void create(Stream &stream, ImageFormat requiredFmt = ImageFormat::None, bool flipVertically = false);
    void create(const std::string &path, ImageFormat requiredFmt = ImageFormat::None, bool flipVertically = false);
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
