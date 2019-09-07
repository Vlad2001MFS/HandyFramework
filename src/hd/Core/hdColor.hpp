#pragma once
#include <cstdint>

namespace hd {

struct Color3 {
    Color3() {
        this->b = 0;
        this->g = 0;
        this->r = 0;
    }
    Color3(uint8_t r, uint8_t g, uint8_t b) {
        this->b = b;
        this->g = g;
        this->r = r;
    }

    bool operator==(const Color3 &rhs) const {
        return this->r == rhs.r && this->g == rhs.g && this->b == rhs.b;
    }

    bool operator!=(const Color3 &rhs) const {
        return this->r != rhs.r || this->g != rhs.g || this->b != rhs.b;
    }

    uint8_t b, g, r;

    static const Color3 None, Black, White, Red, Green, Blue;
};

struct Color4 {
    Color4() {
        this->color = 0;
    }
    explicit Color4(uint32_t color) {
        this->color = color;
    }
    Color4(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        this->b = b;
        this->g = g;
        this->r = r;
        this->a = a;
    }
    Color4(const Color3 &rgb, uint8_t a = 255) {
        this->b = rgb.b;
        this->g = rgb.g;
        this->r = rgb.r;
        this->a = a;
    }

    bool operator==(const Color4 &rhs) const {
        return this->r == rhs.r && this->g == rhs.g && this->b == rhs.b && this->a == rhs.a;
    }

    bool operator!=(const Color4 &rhs) const {
        return this->r != rhs.r || this->g != rhs.g || this->b != rhs.b || this->a != rhs.a;
    }

    const Color3 &getRGB() const { return *reinterpret_cast<const Color3*>(&this->b); }
    Color3 &getRGB() { return *reinterpret_cast<Color3*>(&this->b); }

    union {
        uint32_t color;
        struct {
            uint8_t b, g, r, a;
        };
    };

    static const Color4 None, Black, White, Red, Green, Blue;
};

}
