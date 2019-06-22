#include "hdOSSpecificInfo.hpp"
#include "../hdBuildConfig.hpp"
#include "../../3rd/include/SDL2/SDL_syswm.h"

namespace hd {

void *getSystemWindowHandle(void *sdlWindow) {
    SDL_SysWMinfo info;
    SDL_GetWindowWMInfo(static_cast<SDL_Window*>(sdlWindow), &info);
#if defined(HD_PLATFORM_UNIX)
    return reinterpret_cast<void*>(info.info.x11.window);
#elif defined(HD_PLATFORM_WIN)
    return info.info.win.window;
#endif
}

}
