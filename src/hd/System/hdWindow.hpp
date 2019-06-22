#pragma once
#include "hdWindowEvent.hpp"
#include "../Core/hdCommon.hpp"
#include <memory>
#include <string>

namespace hd {

class Window {
    HD_NONCOPYABLE_CLASS(Window)
public:
    Window();
    Window(const std::string &title, uint32_t w, uint32_t h);
    ~Window();

    void create(const std::string &title, uint32_t w, uint32_t h);
    void destroy();
    bool processEvent(WindowEvent &e);
    void close();

    void setCursorPos(int x, int y);
    int getCursorPosX() const;
    int getCursorPosY() const;
    void setCursorVisible(bool visible);
    bool isCursorVisible() const;
    bool isKeyDown(KeyCode code) const;
    bool isMouseButtonDown(MouseButton button) const;
    bool isOpened() const;
    bool isFocused() const;
    void setTitle(const std::string &title);
    std::string getTitle() const;
    void setPos(int x, int y);
    int getPosX() const;
    int getPosY() const;
    void setSize(uint32_t w, uint32_t h);
    int getSizeX() const;
    int getSizeY() const;
    int getCenterX() const;
    int getCenterY() const;
    void *getOSWindowHandle() const;

    struct Impl;

private:
    std::unique_ptr<Impl> impl;
};

}
