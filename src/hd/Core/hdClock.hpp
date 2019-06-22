#pragma once
#include "hdCommon.hpp"

namespace hd {

class Time {
public:
    Time();
    explicit Time(float raw);
    ~Time();

    Time operator+(const Time &rhs) const;
    Time operator-(const Time &rhs) const;
    Time &operator+=(const Time &rhs);
    Time &operator-=(const Time &rhs);
    bool operator==(const Time &rhs) const;
    bool operator>(const Time &rhs) const;
    bool operator<(const Time &rhs) const;
    bool operator>=(const Time &rhs) const;
    bool operator<=(const Time &rhs) const;

    float getRaw() const;
    float getMilliseconds() const;
    float getSeconds() const;
    float getMinutes() const;
    float getHours() const;
    float getDays() const;

    static Time fromMilliseconds(float time);
    static Time fromSeconds(float time);
    static Time fromMinutes(float time);
    static Time fromHours(float time);
    static Time fromDays(float time);

private:
    float mTime;
};

class Clock {
    HD_STATIC_CLASS(Clock)
public:
    static Time getTime();
    static Time getElapsedTime(const Time &startTime);

};

}
