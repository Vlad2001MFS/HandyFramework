#include "hdLog.hpp"
#include "hdBuildConfig.hpp"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace hd {

Log::Log() {
#ifdef HD_BUILDMODE_DEBUG
    mLevel = LogLevel::Trace;
#else
    mLevel = LogLevel::Error;
#endif
    mLogFile = fopen("app.log", "w");
}

Log::~Log() {
    fclose(static_cast<FILE*>(mLogFile));
}

void Log::write(LogLevel level, const char *file, const char *func, uint32_t line, const char *fmt, ...) {
    if (level < mLevel) {
        return;
    }

    if (level == LogLevel::Warning) {
        mWrite("\n######## WARNING ########\n");
        mWrite("FILE: %s\nFUNCTION: %s\nLINE: %d\n-------------------------\n", file, func, line);
        va_list args;
        va_start(args, fmt);
        mWriteRaw(fmt, args);
        va_end(args);
        mWrite("\n-------------------------\n");
    }
    else if (level == LogLevel::Error) {
        mWrite("\n######## ERROR ########\n");
        mWrite("FILE: %s\nFUNCTION: %s\nLINE: %d\n-------------------------\n", file, func, line);
        va_list args;
        va_start(args, fmt);
        mWriteRaw(fmt, args);
        va_end(args);
        mWrite("\n-------------------------\n");
    }
    else if (level == LogLevel::Trace) {
        mWrite("%s|%s|%d: ", file, func, line);
        va_list args;
        va_start(args, fmt);
        mWriteRaw(fmt, args);
        va_end(args);
    }
    else {
        va_list args;
        va_start(args, fmt);
        mWriteRaw(fmt, args);
        va_end(args);
    }
    mWrite("\n");
}

bool Log::checkAssert(bool expr, const char *exprStr, const char *file, const char *func, uint32_t line) {
    if (!expr) {
        mWrite("\n#### ASSERTION FAILED ####\n");
        mWrite("FILE: %s\nFUNCTION: %s\nLINE: %d\nEXPRESSION: %s", file, func, line, exprStr);
        mWrite("\n");
        return false;
    }
    return true;
}

void Log::setLogLevel(LogLevel level) {
    mLevel = level;
}

void Log::mWrite(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    mWriteRaw(fmt, args);
    va_end(args);
}

void Log::mWriteRaw(const char *fmt, va_list args) {
    char buf[4096] = {0};
    vsprintf_s(buf, 4096, fmt, args);

    fprintf(static_cast<FILE*>(mLogFile), "%s", buf);
    fflush(static_cast<FILE*>(mLogFile));
    fprintf(stdout, "%s", buf);
}

}
