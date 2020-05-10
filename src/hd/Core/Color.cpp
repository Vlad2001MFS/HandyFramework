#include "Color.hpp"

namespace hd {

const Color3 Color3::None  = Color3(0, 0, 0);
const Color3 Color3::Black = Color3(0, 0, 0);
const Color3 Color3::White = Color3(255, 255, 255);
const Color3 Color3::Red   = Color3(255, 0, 0);
const Color3 Color3::Green = Color3(0, 255, 0);
const Color3 Color3::Blue  = Color3(0, 0, 255);

const Color4 Color4::None  = Color4(Color3::None, 0);
const Color4 Color4::Black = Color4(Color3::Black, 255);
const Color4 Color4::White = Color4(Color3::White, 255);
const Color4 Color4::Red   = Color4(Color3::Red, 255);
const Color4 Color4::Green = Color4(Color3::Green, 255);
const Color4 Color4::Blue  = Color4(Color3::Blue, 255);

}
