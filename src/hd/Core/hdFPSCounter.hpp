#pragma once
#include "hdTime.hpp"
#include <cstdint>

namespace hd {

class FPSCounter {
public:
    FPSCounter();

    bool update();
    uint32_t getFps() const;
    float getFrameTime() const;

private:
    uint32_t mTempFps, mFps;
    float mFrameTime;
    Time mTimer;
};

}
