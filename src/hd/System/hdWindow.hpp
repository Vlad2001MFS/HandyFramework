#pragma once
#include "../Core/hdColor.hpp"
#include "hdWindowEvent.hpp"
#include <memory>
#include <string>

namespace hd {

struct OpenGLContextSettings {
    OpenGLContextSettings();
    OpenGLContextSettings(uint32_t majorVersion, uint32_t minorVersion, uint32_t depthBits, uint32_t stencilBits, uint32_t msaaSamples, bool isCoreProfile, bool isDebug);

    uint32_t majorVersion, minorVersion;
    uint32_t depthBits, stencilBits;
    uint32_t msaaSamples;
    bool isCoreProfile;
    bool isDebug;
};

enum class WindowFlags {
    None = 0,
    Fullscreen = 1,
    Hidden = 2,
    Borderless = 4,
    Resizable = 8,
    Minimized = 16,
    Maximized = 32,
    InputGrabbed = 64,
    AllowHighDPI = 128
};
HD_DECL_ENUM_OPS(WindowFlags)

class Window {
    HD_NONCOPYABLE_CLASS(Window)

public:
    Window();
    Window(const std::string &title, uint32_t w, uint32_t h, WindowFlags flags);
    Window(const std::string &title, uint32_t w, uint32_t h, WindowFlags flags, const OpenGLContextSettings &contextSettings);
    ~Window();

    void create(const std::string &title, uint32_t w, uint32_t h, WindowFlags flags);
    void create(const std::string &title, uint32_t w, uint32_t h, WindowFlags flags, const OpenGLContextSettings &contextSettings);
    void destroy();
    void activate();
    bool processEvent(WindowEvent &e);
    void swapBuffers();
    void close();

    void setHighPriorityProcess();
    void setVSyncState(bool vsync);
    bool getVSyncState() const;
    void setCursorPosition(int x, int y);
    int getCursorPositionX() const;
    int getCursorPositionY() const;
    void setCursorVisible(bool visible);
    bool isCursorVisible() const;
    bool isKeyDown(KeyCode code) const;
    bool isMouseButtonDown(MouseButton button) const;
    bool isOpened() const;
    bool isFocused() const;
    void setTitle(const std::string &title);
    std::string getTitle() const;
    void setPosition(int x, int y);
    int getPositionX() const;
    int getPositionY() const;
    void setSize(uint32_t w, uint32_t h);
    int getSizeX() const;
    int getSizeY() const;
    int getCenterX() const;
    int getCenterY() const;
    WindowFlags getFlags() const;
    bool hasOpenGLContext() const;
    const OpenGLContextSettings &getOpenGLContextSettings() const;
    Color4 *getSurface() const;
    void *getOSWindowHandle() const;
    void *getOSContextHandle() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

}
