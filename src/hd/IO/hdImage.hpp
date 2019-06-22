#pragma once
#include "../Core/hdColor.hpp"
#include "hdStream.hpp"

namespace hd {

class Image {
public:
    Image();
    Image(const Color4 *data, uint32_t w, uint32_t h);
    explicit Image(StreamReader &stream);
    explicit Image(const std::string &filename);
    Image(const Image &img);
    ~Image();

    Image &operator=(const Image &rhs);

    void create(const Color4 *data, uint32_t w, uint32_t h);
    void create(StreamReader &stream);
    void create(const std::string &filename);
    void destroy();
    uint32_t getWidth() const { return mWidth; }
    uint32_t getHeight() const { return mHeight; }
    const Color4 *getPixels() const { return mData; }
    Color4 *getPixels() { return mData; }
    Color4 getPixel(int x, int y) const { return (x > -1 && x < mWidth && y > -1 && y < mHeight) ? mData[x + y*mWidth] : Color4::Black; }
    void setPixel(const Color4 &color, int x, int y) { if (x > -1 && x < mWidth && y > -1 && y < mHeight) { mData[x + y*mWidth] = color; } }

private:
    Color4 *mData;
    uint32_t mWidth, mHeight;
};

}
