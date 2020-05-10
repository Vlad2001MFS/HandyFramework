#include "Time.hpp"
#include <chrono>

namespace hd {

Time::Time() {
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
    return mTime.count() - rhs.mTime.count();
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

uint64_t Time::getMicroseconds() const {
    return std::chrono::duration_cast<std::chrono::microseconds>(mTime).count();
}

uint64_t Time::getMilliseconds() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(mTime).count();
}

uint64_t Time::getSeconds() const {
    return std::chrono::duration_cast<std::chrono::seconds>(mTime).count();
}

uint64_t Time::getMinutes() const {
    return std::chrono::duration_cast<std::chrono::minutes>(mTime).count();
}

uint64_t Time::getHours() const {
    return std::chrono::duration_cast<std::chrono::hours>(mTime).count();
}

Time Time::fromMicroseconds(uint64_t time) {
    return Time(std::chrono::microseconds(time));
}

Time Time::fromMilliseconds(uint64_t time) {
    return Time(std::chrono::milliseconds(time));
}

Time Time::fromSeconds(uint64_t time) {
    return Time(std::chrono::seconds(time));
}

Time Time::fromMinutes(uint64_t time) {
    return Time(std::chrono::minutes(time));
}

Time Time::fromHours(uint64_t time) {
    return Time(std::chrono::hours(time));
}

Time Time::getCurrentTime() {
    return Time(std::chrono::high_resolution_clock::now().time_since_epoch());
}

Time Time::getElapsedTime(const Time &startTime) {
    return getCurrentTime() - startTime;
}

Time::Time(const std::chrono::high_resolution_clock::duration &time) : mTime(time) {
}

}
