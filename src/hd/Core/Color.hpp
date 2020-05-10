#pragma once
#include <cstdint>

namespace hd {

struct Color3 {
    Color3() {
        this->r = 0;
        this->g = 0;
        this->b = 0;
    }
    Color3(uint8_t r, uint8_t g, uint8_t b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    bool operator==(const Color3 &rhs) const {
        return this->r == rhs.r && this->g == rhs.g && this->b == rhs.b;
    }

    bool operator!=(const Color3 &rhs) const {
        return this->r != rhs.r || this->g != rhs.g || this->b != rhs.b;
    }

    uint8_t r, g, b;

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
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
    Color4(const Color3 &rgb, uint8_t a = 255) {
        this->r = rgb.r;
        this->g = rgb.g;
        this->b = rgb.b;
        this->a = a;
    }

    bool operator==(const Color4 &rhs) const {
        return this->r == rhs.r && this->g == rhs.g && this->b == rhs.b && this->a == rhs.a;
    }

    bool operator!=(const Color4 &rhs) const {
        return this->r != rhs.r || this->g != rhs.g || this->b != rhs.b || this->a != rhs.a;
    }

    const Color3 &getRGB() const { return *reinterpret_cast<const Color3*>(this); }
    Color3 &getRGB() { return *reinterpret_cast<Color3*>(this); }

    union {
        uint32_t color;
        struct {
            uint8_t r, g, b, a;
        };
    };

    static const Color4 None, Black, White, Red, Green, Blue;
};

}
