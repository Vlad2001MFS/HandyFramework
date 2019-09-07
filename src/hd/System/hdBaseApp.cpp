#include "hdBaseApp.hpp"

namespace hd {

void BaseApp::onInitialize() {}
void BaseApp::onShutdown() {}
void BaseApp::onEvent(const WindowEvent &event) {}
void BaseApp::onFixedUpdate() {}
void BaseApp::onUpdate() {}
void BaseApp::onDraw() {}

void BaseApp::run() {
    const float UPDATES_COUNT_PER_SEC = 30.0f;
    const Time UPDATE_TIME = Time::fromMilliseconds(1000.0f / UPDATES_COUNT_PER_SEC);

    onInitialize();
    Time updateTimer;
    bool isExit = false;
    while (!isExit) {
        WindowEvent event;
        while (mWindow.processEvent(event)) {
            if (event.type == WindowEventType::Close) {
                isExit = true;
            }
            if (mWindow.isFocused()) {
                onEvent(event);
            }
        }

        if (mWindow.isFocused()) {
            if (Time::getElapsedTime(updateTimer) > UPDATE_TIME) {
                onFixedUpdate();
                updateTimer = Time::getCurrentTime();
            }
            onUpdate();
            onDraw();

            mFPSCounter.update();
        }
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
