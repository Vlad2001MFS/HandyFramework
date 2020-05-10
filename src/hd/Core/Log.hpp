#pragma once
#include "Common.hpp"
#include "fmt/format.h"

#define HD_LOG_INFO(fmt, ...) \
    hd::Log::get().write(hd::LogLevel::Info, __FILE__, __FUNCSIG__ , __LINE__, (fmt), ##__VA_ARGS__)
#define HD_LOG_WARNING(fmt, ...) \
    hd::Log::get().write(hd::LogLevel::Warning, __FILE__, __FUNCSIG__ , __LINE__, (fmt), ##__VA_ARGS__)
#define HD_LOG_ERROR(fmt, ...) \
    hd::Log::get().write(hd::LogLevel::Error, __FILE__, __FUNCSIG__ , __LINE__, (fmt), ##__VA_ARGS__);
#define HD_LOG_FATAL(fmt, ...) \
    hd::Log::get().write(hd::LogLevel::Fatal, __FILE__, __FUNCSIG__ , __LINE__, (fmt), ##__VA_ARGS__);
#define HD_ASSERT(expr) \
    hd::Log::get().checkAssert((expr), #expr, __FILE__, __FUNCSIG__ , __LINE__)

namespace hd {

enum class LogLevel {
    Info,
    Warning,
    Error,
    Fatal
};

class Log : public Singleton<Log> {
public:
    Log();

    void appAtEntryPoint(int argc, char **argv);

    template<typename... Args>
    void write(LogLevel level, const char *file, const char *func, uint32_t line, const char *fmt, const Args &...args) {
        writeArgs(level, file, func, line, fmt, fmt::make_format_args(args...));
    }

    void writeArgs(LogLevel level, const char *file, const char *func, uint32_t line, const char *fmt, fmt::format_args args);
    void checkAssert(bool expr, const char *exprStr, const char *file, const char *func, uint32_t line);
};

}