#include "Image.hpp"
#include "FileStream.hpp"
#include "../Core/Log.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "../../stb/stb_image.h"

namespace hd {

Image::Image() : mSize(0, 0) {
    mFmt = ImageFormat::None;
}

Image::Image(const void *data, const glm::ivec2 &size, ImageFormat fmt) : Image() {
    create(data, size, fmt);
}

Image::Image(Stream &stream, ImageFormat requiredFmt, bool flipVertically) : Image() {
    create(stream, requiredFmt, flipVertically);
}

Image::Image(const std::string &path, ImageFormat requiredFmt, bool flipVertically) : Image() {
    create(path, requiredFmt, flipVertically);
}

Image::~Image() {
    destroy();
}

void Image::create(const void *data, const glm::ivec2 &size, ImageFormat fmt) {
    destroy();
    HD_ASSERT(size.x > 0);
    HD_ASSERT(size.y > 0);
    HD_ASSERT(fmt != ImageFormat::None);

    size_t dataSize = static_cast<size_t>(size.x*size.y*static_cast<int>(fmt));
    mData.resize(dataSize);
    mSize = size;
    mFmt = fmt;

    if (data) {
        memcpy(mData.data(), data, dataSize);
    }
}

void Image::create(Stream &stream, ImageFormat requiredFmt, bool flipVertically) {
    HD_ASSERT(stream.isReadable());

    mPath = stream.getName();

    stbi_io_callbacks callbacks;
    callbacks.read = [](void *userdata, char *data, int size) {
        Stream *stream = static_cast<Stream*>(userdata);
        return static_cast<int>(stream->read(data, static_cast<size_t>(size)));
    };
    callbacks.skip = [](void *userdata, int size) {
        Stream *stream = static_cast<Stream*>(userdata);
        stream->seek(stream->tell() + static_cast<size_t>(size));
    };
    callbacks.eof = [](void *userdata) {
        Stream *stream = static_cast<Stream*>(userdata);
        return static_cast<int>(stream->isEOF());
    };

    stbi_set_flip_vertically_on_load(flipVertically);
    
    int width, height, components;
    uint8_t *data = stbi_load_from_callbacks(&callbacks, &stream, &width, &height, &components, static_cast<int>(requiredFmt));
    if (!data) {
        HD_LOG_FATAL("Failed to load image from stream '{}'. Error: {}", stream.getName().data(), stbi_failure_reason());
    }
    if (requiredFmt != ImageFormat::None) {
        HD_ASSERT(components == static_cast<int>(requiredFmt));
        create(data, glm::ivec2(width, height), requiredFmt);
    }
    else {
        create(data, glm::ivec2(width, height), static_cast<ImageFormat>(components));
    }
    stbi_image_free(data);
}

void Image::create(const std::string &path, ImageFormat requiredFmt, bool flipVertically) {
    FileStream fs(path, FileMode::Read);
    create(fs, requiredFmt, flipVertically);
}

void Image::destroy() {
    mData.clear();
    mSize = glm::ivec2(0, 0);
    mFmt = ImageFormat::None;
}

const void *Image::getData() const {
    return mData.data();
}

void *Image::getData() {
    return mData.data();
}

const glm::ivec2 &Image::getSize() const {
    return mSize;
}

ImageFormat Image::getFormat() const {
    return mFmt;
}

const std::string &Image::getPath() const {
    return mPath;
}

}
