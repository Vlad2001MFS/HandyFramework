#pragma once
#include "hdWindowEvent.hpp"
#include "../Core/hdMacros.hpp"
#include "../Core/hdColor.hpp"
#include "glm/glm.hpp"
#include <memory>
#include <string>

namespace hd {

struct OpenGLContextSettings final {
    OpenGLContextSettings();
    OpenGLContextSettings(uint32_t majorVersion, uint32_t minorVersion, uint32_t depthBits, uint32_t stencilBits,
        uint32_t msaaSamples, bool isCoreProfile, bool isDebug);

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

class Window final {
    HD_NONCOPYABLE_CLASS(Window);
public:
    Window();
    Window(const std::string &title, const glm::ivec2 &size, WindowFlags flags);
    Window(const std::string &title, const glm::ivec2 &size, WindowFlags flags,
        const OpenGLContextSettings &contextSettings);
    ~Window();

    void create(const std::string &title, const glm::ivec2 &size, WindowFlags flags);
    void create(const std::string &title, const glm::ivec2 &size, WindowFlags flags,
        const OpenGLContextSettings &contextSettings);
    void destroy();
    bool processEvent(WindowEvent &e);
    void swapBuffers();
    void close();
    void activate();
    void *lockSurface();
    void unlockSurface();

    void setTitle(const std::string &title);
    void setPosition(const glm::ivec2 &pos);
    void setSize(const glm::ivec2 &size);
    void setCursorPosition(const glm::ivec2 &pos);
    void setCursorVisible(bool visible);
    void setVSyncState(bool vsync);

    std::string getTitle() const;
    glm::ivec2 getPosition() const;
    glm::ivec2 getSize() const;
    glm::ivec2 getCenter() const;
    WindowFlags getFlags() const;
    void *getNativeWindowHandle() const;
    bool isFocused() const;
    glm::ivec2 getCursorPosition() const;
    bool isCursorVisible() const;
    bool isKeyDown(KeyCode code) const;
    bool isKeyDown(MouseButton button) const;
    bool getVSyncState() const;
    const OpenGLContextSettings &getOpenGLContextSettings() const;
    void *getNativeGLContextHandle() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

}
