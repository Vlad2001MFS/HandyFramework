#include "hdImage.hpp"
#include "hdFileStream.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../../3rd/include/stb/stb_image.h"
#include <memory>
#include <vector>

namespace hd {

Image::Image() {
    mData = nullptr;
    mWidth = 0;
    mHeight = 0;
}

Image::Image(const Color4 *data, uint32_t w, uint32_t h) : Image() {
    create(data, w, h);
}

Image::Image(StreamReader &stream) : Image() {
    create(stream);
}

Image::Image(const std::string &filename) : Image() {
    create(filename);
}

Image::Image(const Image &img) : Image() {
    create(img.getPixels(), img.getWidth(), img.getHeight());
}

Image::~Image() {
    destroy();
}

Image &Image::operator=(const Image &rhs) {
    create(rhs.getPixels(), rhs.getWidth(), rhs.getHeight());
    return *this;
}

void Image::create(const Color4 *data, uint32_t w, uint32_t h) {
    HD_ASSERT(w != 0);
    HD_ASSERT(h != 0);
    destroy();
    uint32_t count = w*h;
    mData = new Color4[count];
    if (data) {
        memcpy(mData, data, sizeof(Color4)*count);
    }
    else {
        memset(mData, 0, sizeof(Color4)*count);
    }
    mWidth = w;
    mHeight = h;
}

void Image::create(StreamReader &stream) {
    std::vector<uint8_t> buf(stream.getSize());
    stream.read(buf.data(), stream.getSize());
    int w, h, channels;
    uint8_t *data = stbi_load_from_memory(buf.data(), static_cast<int>(buf.size()), &w, &h, &channels, 4);
    if (!data) {
        HD_LOG_ERROR("Failed to load image from stream '%s'", stream.getName().data());
    }
    mData = reinterpret_cast<Color4*>(data);
    mWidth = w;
    mHeight = h;
    mPath = stream.getName();
}

void Image::create(const std::string &filename) {
    FileReader fs(filename);
    create(fs);
}

void Image::destroy() {
    stbi_image_free(mData);
    mWidth = 0;
    mHeight = 0;
}

void Image::flipVertical() {
    hd::Color4 *buf = new hd::Color4[mWidth*mHeight];
    memcpy(buf, mData, sizeof(hd::Color4)*mWidth*mHeight);
    for (size_t y = 0; y < mHeight; y++) {
        for (size_t x = 0; x < mWidth; x++) {
            mData[y*mWidth + x] = buf[(mWidth - y - 1)*mWidth + x];
        }
    }
    HD_DELETE_ARRAY(buf);
}

const std::string& Image::getPath() const {
    return mPath;
}

}
