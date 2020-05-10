#include "FPSCounter.hpp"

namespace hd {

FPSCounter::FPSCounter() : mTimer(Time::getCurrentTime()) {
    this->mTempFps = 0;
    this->mFps = 0;
    this->mFrameTime = 0.0F;
}

bool FPSCounter::update() {
    mFrameTime = hd::Time::getElapsedTime(mDeltaTimer).getMicroseconds()*0.001f;
    mDeltaTimer = hd::Time::getCurrentTime();

    Time elapsedTime = Time::getElapsedTime(mTimer);
    if (elapsedTime >= Time::fromMilliseconds(1000)) {
        mFps = mTempFps;
        mTempFps = 1;
        mTimer = Time::getCurrentTime();
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
