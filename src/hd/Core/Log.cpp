#include "Log.hpp"
#include "../../loguru/loguru.hpp"
#include "StringUtils.hpp"
#include <vector>

namespace hd {

Log::Log() {
    FILE *f = fopen("/proc/self/cmdline", "rb");
    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    std::string cmdline;
    cmdline.resize(fileSize + 1);
    fread(cmdline.data(), 1, fileSize, f);
    fclose(f);

    std::vector<std::string> args = hd::StringUtils::split(cmdline, " ", false);
    int argc = args.size();
    std::vector<char*> argv;
    for (auto &arg : args) {
        argv.push_back(arg.data());
    }

    loguru::init(argc, argv.data());
    loguru::add_file("App.log", loguru::FileMode::Truncate, loguru::Verbosity_MAX);
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