#pragma once
#include "../Core/hdFPSCounter.hpp"
#include "hdWindow.hpp"

#define HD_APP_MAIN(appClass) int main(int argc, char **argv) { \
    HD_UNUSED(argc); \
    HD_UNUSED(argv); \
    appClass hd_main_app;\
    hd_main_app.run();\
    return 0;\
}

namespace hd {

class BaseApp {
public:
    BaseApp();
    virtual ~BaseApp();

    virtual void onInitialize();
    virtual void onShutdown();
    virtual void onEvent(const WindowEvent &event);
    virtual void onFixedUpdate();
    virtual void onUpdate();
    virtual void onDraw();

    void run();
    uint32_t getFps() const;
    float getFrameTime() const;

protected:
    Window mWindow;

private:
    FPSCounter mFPSCounter;
};
    
}
