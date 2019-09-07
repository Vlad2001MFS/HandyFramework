#pragma once
#include "hdImage.hpp"
#include "../Core/hdColor.hpp"
#include <memory>

namespace hd {

enum class FontStyle {
    Normal,
    Bold,
    Italic,
    Underline,
    Strikethrough
};

enum class FontHinting {
    Normal,
    Light,
    Mono,
    None
};

class Font {
    HD_NONCOPYABLE_CLASS(Font);
public:
    Font();
    Font(const std::string &filename, uint32_t size);
    ~Font();

    void create(const std::string &filename, uint32_t size);
    void destroy();
    
    uint32_t calcTextWidth(const std::string &text) const;
    Image renderLine(const std::string &text, const Color4 &color) const;

    void setStyle(FontStyle style);
    void setOutline(uint32_t outline);
    void setHinting(FontHinting hinting);
    void setKerning(bool kerning);
    uint32_t getMaxGlyphHeight() const;
    uint32_t getLineHeight() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

}
