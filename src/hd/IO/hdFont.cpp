#include "hdFont.hpp"
#include "hdFileStream.hpp"
#include "../../3rd/include/SDL2/SDL_ttf.h"

namespace hd {

static const int gFontStyles[] = {
    TTF_STYLE_NORMAL,
    TTF_STYLE_BOLD,
    TTF_STYLE_ITALIC,
    TTF_STYLE_UNDERLINE,
    TTF_STYLE_STRIKETHROUGH
};

static const int gFontHintings[] = {
    TTF_STYLE_NORMAL,
    TTF_HINTING_LIGHT,
    TTF_HINTING_MONO,
    TTF_HINTING_NONE
};

struct Font::Impl {
    TTF_Font *font = nullptr;
    FontStyle style = FontStyle::Normal;
    FontHinting hinting = FontHinting::Normal;
    uint32_t outline = 0;
    bool kerning = true;

    void convertFormat(SDL_Surface *&surface) {
        SDL_Surface *converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(surface);
        surface = converted;
    }
};

Font::Font() : impl(new Impl()) {
}

Font::Font(const std::string &filename, uint32_t size) : Font() {
	create(filename, size);
}

Font::~Font() {
	destroy();
}

void Font::create(const std::string &filename, uint32_t size) {
    // TTF_Init and TTF_Quit implementing a reference counter inside, therefore SDL_TTF will not initialize several times
    if (TTF_Init() != 0) {
        HD_LOG_ERROR("Failed to initizize SDL_ttf. Errors: %s", TTF_GetError());
    }
    impl->font = TTF_OpenFont(filename.data(), static_cast<int>(size));
    if (!impl->font) {
        TTF_Quit();
        HD_LOG_ERROR("Failed to create font. Error: %s", TTF_GetError());
    }
    setHinting(FontHinting::Mono);
    setKerning(false);
}

void Font::destroy() {
	TTF_CloseFont(impl->font);
    TTF_Quit();
    *impl = Impl();
}

uint32_t Font::calcTextWidth(const std::string &text) const {
    int width;
	TTF_SizeText(impl->font, text.data(), &width, nullptr);
    return static_cast<uint32_t>(width);
}

Image Font::renderLine(const std::string &text, const Color4 &color) const {
	SDL_Surface *surface = TTF_RenderText_Solid(impl->font, text.data(), { color.r, color.g, color.b, color.a });
    if (!surface) {
        TTF_CloseFont(impl->font);
        TTF_Quit();
        HD_LOG_ERROR("Failed to render line '%s'. Error: %s", text.data(), TTF_GetError());
    }
    impl->convertFormat(surface);

    Image img(static_cast<const Color4*>(surface->pixels), surface->w, surface->h);
    SDL_FreeSurface(surface);

    hd::Color4 bg = img.getPixel(0, 0);
    hd::Color4 newBg = hd::Color4::None;
    for (int i = 0; i < img.getWidth()*img.getHeight(); i++) {
        hd::Color4 &pixel = img.getPixels()[i];
        if (pixel.getRGB() == bg.getRGB()) {
            pixel = newBg;
        }
        else if (pixel.a == 0) {
            pixel = color;
        }
    }
    hd::Color4 stokeColor = hd::Color4(0, 0, 0, 160);
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            if (img.getPixel(x, y).getRGB() == color.getRGB()) {
                for (int ny = -1; ny <= 1; ny++) {
                    for (int nx = -1; nx <= 1; nx++) {
                        if (img.getPixel(x + nx, y + ny).getRGB() != color.getRGB()) {
                            img.setPixel(stokeColor, x + nx, y + ny);
                        }
                    }
                }
            }
        }
    }
    return img;
}

void Font::setStyle(FontStyle style) {
	if (impl->style != style) {
        impl->style = style;
        TTF_SetFontStyle(impl->font, gFontStyles[static_cast<size_t>(style)]);
    }
}

void Font::setOutline(uint32_t outline) {
	if (impl->outline != outline) {
        impl->outline = outline;
        TTF_SetFontOutline(impl->font, static_cast<int>(outline));
    }
}

void Font::setHinting(FontHinting hinting) {
	if (impl->hinting != hinting) {
        impl->hinting = hinting;
        TTF_SetFontHinting(impl->font, gFontHintings[static_cast<size_t>(hinting)]);
    }
}

void Font::setKerning(bool kerning) {
	if (impl->kerning != kerning) {
        impl->kerning = kerning;
        TTF_SetFontKerning(impl->font, kerning);
    }
}

uint32_t Font::getMaxGlyphHeight() const {
    return static_cast<uint32_t>(TTF_FontHeight(impl->font));
}

uint32_t Font::getLineHeight() const {
    return static_cast<uint32_t>(TTF_FontLineSkip(impl->font));
}
    
}
