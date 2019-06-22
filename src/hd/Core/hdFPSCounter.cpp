#include "hdFPSCounter.hpp"

namespace hd {

FPSCounter::FPSCounter() : mTimer(Clock::getTime()) {
    this->mTempFps = 0;
    this->mFps = 0;
    this->mFrameTime = 0.0f;
}

bool FPSCounter::update() {
    auto elapsedTime = Clock::getElapsedTime(mTimer);
    if (elapsedTime >= Time::fromMilliseconds(1000)) {
        mFps = mTempFps;
        mTempFps = 1;
        mFrameTime = elapsedTime.getMilliseconds() / static_cast<float>(mFps);
        mTimer = Clock::getTime();
        return true;
    }
    mTempFps++;
    return false;
}

uint32_t FPSCounter::getFps() const {
    return mFps;
}

float FPSCounter::getFrameTime() const {
    return mFrameTime;
}

}
