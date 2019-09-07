#pragma once
#include "hdMacros.hpp"
#include <cstdlib>

#define HD_LOG_TRACE(fmt, ...) \
    hd::Log::getInstance().write(hd::LogLevel::Trace, __FILE__, __FUNCSIG__ , __LINE__, (fmt), ##__VA_ARGS__)
#define HD_LOG_INFO(fmt, ...) \
    hd::Log::getInstance().write(hd::LogLevel::Info, __FILE__, __FUNCSIG__ , __LINE__, (fmt), ##__VA_ARGS__)
#define HD_LOG_WARNING(fmt, ...) \
    hd::Log::getInstance().write(hd::LogLevel::Warning, __FILE__, __FUNCSIG__ , __LINE__, (fmt), ##__VA_ARGS__)
#define HD_LOG_ERROR(fmt, ...) \
    do { \
        hd::Log::getInstance().write(hd::LogLevel::Error, __FILE__, __FUNCSIG__ , __LINE__, (fmt), ##__VA_ARGS__); \
        HD_LOG_INFO("\n"); \
        HD_LOG_INFO("######################"); \
        HD_LOG_INFO("### App was killed ###"); \
        HD_LOG_INFO("######################"); \
        std::abort(); \
    } while (false)
#define HD_ASSERT(expr) \
    do { if (!hd::Log::getInstance().checkAssert((expr), #expr, __FILE__, __FUNCSIG__ , __LINE__)) { \
        std::abort(); \
    } } while (false)

namespace hd {

enum class LogLevel {
    Trace,
    Info,
    Warning,
    Error
};

class Log final {
    HD_SINGLETON_CLASS(Log);
public:
    void write(LogLevel level, const char *file, const char *func, uint32_t line, const char *fmt, ...);
    bool checkAssert(bool expr, const char *exprStr, const char *file, const char *func, uint32_t line);

    void setLogLevel(LogLevel level);

private:
    void mWrite(const char *fmt, ...);
    void mWriteRaw(const char *fmt, va_list args);

    LogLevel mLevel;
    void *mLogFile;
};

}