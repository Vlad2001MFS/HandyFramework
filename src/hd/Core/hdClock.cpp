#include "hdClock.hpp"
#include <ctime>
#include <limits>

namespace hd {

Time::Time() {
    mTime = 0.0f;
}

Time::Time(float raw) {
    mTime = raw;
}

Time::~Time() {
}

Time Time::operator+(const Time &rhs) const {
    return Time(mTime + rhs.mTime);
}

Time Time::operator-(const Time &rhs) const {
    return Time(mTime - rhs.mTime);
}

Time &Time::operator+=(const Time &rhs) {
    mTime += rhs.mTime;
    return *this;
}

Time &Time::operator-=(const Time &rhs) {
    mTime -= rhs.mTime;
    return *this;
}

bool Time::operator==(const Time &rhs) const {
    return std::abs(mTime - rhs.mTime) < std::numeric_limits<float>::epsilon();
}

bool Time::operator>(const Time &rhs) const {
    return mTime > rhs.mTime;
}

bool Time::operator<(const Time &rhs) const {
    return mTime < rhs.mTime;
}

bool Time::operator>=(const Time &rhs) const {
    return mTime >= rhs.mTime;
}

bool Time::operator<=(const Time &rhs) const {
    return mTime <= rhs.mTime;
}

float Time::getRaw() const {
    return mTime;
}

float Time::getMilliseconds() const {
    return getSeconds()*1000.0f;
}

float Time::getSeconds() const {
    return mTime / CLOCKS_PER_SEC;
}

float Time::getMinutes() const {
    return getSeconds() / 60.0f;
}

float Time::getHours() const {
    return getMinutes() / 60.0f;
}

float Time::getDays() const {
    return getHours() / 24.0f;
}

Time Time::fromMilliseconds(float time) {
    return fromSeconds(time / 1000.0f);
}

Time Time::fromSeconds(float time) {
    return Time(time*CLOCKS_PER_SEC);
}

Time Time::fromMinutes(float time) {
    return fromSeconds(time*60.0f);
}

Time Time::fromHours(float time) {
    return fromMinutes(time*60.0f);
}

Time Time::fromDays(float time) {
    return fromHours(time*24.0f);
}

Time Clock::getTime() {
    return Time(static_cast<float>(clock()));
}

Time Clock::getElapsedTime(const Time &startTime) {
    return Time(static_cast<float>(clock())) - startTime;
}

}
