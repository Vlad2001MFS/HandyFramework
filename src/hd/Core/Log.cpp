#include "Log.hpp"
#include "../../loguru/loguru.hpp"
#include "StringUtils.hpp"
#include <vector>

namespace hd {

Log::Log() {
    loguru::add_file("App.log", loguru::FileMode::Truncate, loguru::Verbosity_MAX);
}

void Log::appAtEntryPoint(int argc, char **argv) {
    HD_LOG_INFO("APP ENTRY POINT");
    loguru::init(argc, argv);
    loguru::add_file("App.log", loguru::FileMode::Append, loguru::Verbosity_MAX);
}

void Log::writeArgs(LogLevel level, const char *file, const char *func, uint32_t line, const char *fmt, fmt::format_args args) {
    switch (level) {
        case LogLevel::Info: {
            loguru::vlog(loguru::Verbosity_INFO, file, line, fmt, args);
            break;
        }
        case LogLevel::Warning: {
            loguru::vlog(loguru::Verbosity_WARNING, file, line, fmt, args);
            break;
        }
        case LogLevel::Error: {
            loguru::vlog(loguru::Verbosity_ERROR, file, line, fmt, args);
            break;
        }
        case LogLevel::Fatal: {
            loguru::vlog(loguru::Verbosity_FATAL, file, line, fmt, args);
            break;
        }
    }
}

void Log::checkAssert(bool expr, const char *exprStr, const char *file, const char *func, uint32_t line) {
    if (!expr) {
        loguru::log_and_abort(0, fmt::format("CHECK FAILED:  {}  ", exprStr).data(), file, line);
    }
}

}
