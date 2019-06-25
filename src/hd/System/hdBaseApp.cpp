#include "hdBaseApp.hpp"

namespace hd {

BaseApp::BaseApp() {}
BaseApp::~BaseApp() {}

void BaseApp::onInitialize() {}
void BaseApp::onShutdown() {}
void BaseApp::onEvent(const WindowEvent &event) {}
void BaseApp::onFixedUpdate() {}
void BaseApp::onUpdate() {}
void BaseApp::onDraw() {}

void BaseApp::run() {
    const auto UPDATES_COUNT_PER_SEC = 30.0f;
    const auto UPDATE_TIME = Time::fromMilliseconds(1000.0f / UPDATES_COUNT_PER_SEC);

    onInitialize();
    Time updateTimer;
    auto isExit = false;
    while (!isExit) {
        WindowEvent event;
        while (mWindow.processEvent(event)) {
            if (event.type == hd::WindowEventType::Close) {
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
