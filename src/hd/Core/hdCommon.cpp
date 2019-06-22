#include "hdCommon.hpp"
#include "hdStringUtils.hpp"
#if defined(HD_PLATFORM_WIN)
    #define WIN32_MEAN_AND_LEAN
    #include "windows.h"
#endif
#include <cstdarg>

namespace hd {

static const char gLogFilename[] = "Application.log";

struct _LogReset {
    _LogReset() {
        fclose(fopen(gLogFilename, "w"));
    }
} _gLogReset;

void debugLog(const char *prefix, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stdout, prefix);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
    auto f = fopen(gLogFilename, "a");
    fprintf(f, prefix);
    vfprintf(f, fmt, args);
    fprintf(f, "\n");
    fclose(f);
    va_end(args);
}

}
