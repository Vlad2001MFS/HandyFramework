#include "hdBaseApp.hpp"

namespace hd {

BaseApp::BaseApp() {}
BaseApp::~BaseApp() {}

void BaseApp::run() {
    const auto UPDATES_COUNT_PER_SEC = 30.0f;
    const auto UPDATE_TIME = Time::fromMilliseconds(1000.0f / UPDATES_COUNT_PER_SEC);

    onInitialize();
    Time updateTimer;
    auto isExit = false;
    while (!isExit) {
        WindowEvent event;
        while (mWindow.processEvent(event)) {
            if (event.type == WindowEventType::Close || (event.type == WindowEventType::Key && event.key.code == KeyCode::Escape)) {
                isExit = true;
            }
            onEvent(event);
        }

        if (Clock::getElapsedTime(updateTimer) > UPDATE_TIME) {
            onFixedUpdate();
            updateTimer = Clock::getTime();
        }
        onUpdate();
        onDraw();

        mFPSCounter.update();
    }
    onShutdown();
}

uint32_t BaseApp::getFps() const {
    return mFPSCounter.getFps();
}

float BaseApp::getFrameTime() const {
    return mFPSCounter.getFrameTime();
}

}
