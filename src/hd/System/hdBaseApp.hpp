#pragma once
#include "hdWindow.hpp"
#include "../Core/hdFPSCounter.hpp"

#define HD_APP_MAIN(appClass) int main(int argc, char **argv) { \
    appClass().run();\
    return 0;\
}

namespace hd {

class BaseApp {
public:
    BaseApp() = default;
    virtual ~BaseApp() = default;

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
