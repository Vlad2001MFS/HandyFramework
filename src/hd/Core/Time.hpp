#pragma once
#include <chrono>

namespace hd {

class Time {
public:
    Time();

    Time operator+(const Time &rhs) const;
    Time operator-(const Time &rhs) const;
    Time &operator+=(const Time &rhs);
    Time &operator-=(const Time &rhs);
    bool operator==(const Time &rhs) const;
    bool operator>(const Time &rhs) const;
    bool operator<(const Time &rhs) const;
    bool operator>=(const Time &rhs) const;
    bool operator<=(const Time &rhs) const;

    uint64_t getMicroseconds() const;
    uint64_t getMilliseconds() const;
    uint64_t getSeconds() const;
    uint64_t getMinutes() const;
    uint64_t getHours() const;

    static Time fromMicroseconds(uint64_t time);
    static Time fromMilliseconds(uint64_t time);
    static Time fromSeconds(uint64_t time);
    static Time fromMinutes(uint64_t time);
    static Time fromHours(uint64_t time);

    static Time getCurrentTime();
    static Time getElapsedTime(const Time &startTime);

private:
    explicit Time(const std::chrono::high_resolution_clock::duration &time);

    std::chrono::high_resolution_clock::duration mTime;
};

}
