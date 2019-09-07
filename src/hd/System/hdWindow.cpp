#include "hdWindow.hpp"
#include "../Core/hdLog.hpp"
#include "../../3rd/include/SDL2/SDL.h"
#include "../../3rd/include/SDL2/SDL_syswm.h"

namespace hd {

static const SDL_Scancode gKeyCodes[] =
{
    SDL_SCANCODE_UNKNOWN,

    SDL_SCANCODE_A, SDL_SCANCODE_B, SDL_SCANCODE_C, SDL_SCANCODE_D, SDL_SCANCODE_E,
    SDL_SCANCODE_F, SDL_SCANCODE_G, SDL_SCANCODE_H, SDL_SCANCODE_I, SDL_SCANCODE_J,
    SDL_SCANCODE_K, SDL_SCANCODE_L, SDL_SCANCODE_M, SDL_SCANCODE_N, SDL_SCANCODE_O,
    SDL_SCANCODE_P, SDL_SCANCODE_Q, SDL_SCANCODE_R, SDL_SCANCODE_S, SDL_SCANCODE_T,
    SDL_SCANCODE_U, SDL_SCANCODE_V, SDL_SCANCODE_W, SDL_SCANCODE_X, SDL_SCANCODE_Y,
    SDL_SCANCODE_Z,

    SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9,

    SDL_SCANCODE_RETURN, SDL_SCANCODE_ESCAPE, SDL_SCANCODE_BACKSPACE,
    SDL_SCANCODE_TAB, SDL_SCANCODE_SPACE,

    SDL_SCANCODE_F1, SDL_SCANCODE_F2, SDL_SCANCODE_F3, SDL_SCANCODE_F4, SDL_SCANCODE_F5,
    SDL_SCANCODE_F6, SDL_SCANCODE_F7, SDL_SCANCODE_F8, SDL_SCANCODE_F9, SDL_SCANCODE_F10,
    SDL_SCANCODE_F11, SDL_SCANCODE_F12,

    SDL_SCANCODE_PAUSE, SDL_SCANCODE_INSERT, SDL_SCANCODE_HOME, SDL_SCANCODE_PAGEUP, SDL_SCANCODE_PAGEDOWN,
    SDL_SCANCODE_DELETE, SDL_SCANCODE_END, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN,

    SDL_SCANCODE_KP_0, SDL_SCANCODE_KP_1, SDL_SCANCODE_KP_2, SDL_SCANCODE_KP_3, SDL_SCANCODE_KP_4,
    SDL_SCANCODE_KP_5, SDL_SCANCODE_KP_6, SDL_SCANCODE_KP_7, SDL_SCANCODE_KP_8, SDL_SCANCODE_KP_9,

    SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL, SDL_SCANCODE_LSHIFT, SDL_SCANCODE_RSHIFT,
    SDL_SCANCODE_LALT, SDL_SCANCODE_RALT, SDL_SCANCODE_LGUI, SDL_SCANCODE_RGUI,
};

static const uint32_t gMouseButtons[] = {
    SDL_BUTTON_LEFT,
    SDL_BUTTON_RIGHT,
    SDL_BUTTON_MIDDLE,
    SDL_BUTTON_X1,
    SDL_BUTTON_X2
};

SDL_WindowFlags getSDLWindowFlagsFromWindowFlags(WindowFlags flags);
KeyCode getKeyCodeFromSDLScanCode(SDL_Scancode key);

struct Window::Impl {
    SDL_Window *window = nullptr;
    SDL_GLContext glContext = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
    bool isSurfaceLocked = false;
    bool isNeedSurfaceResize = true;
    OpenGLContextSettings contextSettings;
    WindowFlags flags;
    bool isFocused = true;

    void create(const std::string &title, const glm::ivec2 &size, WindowFlags flags,
        const OpenGLContextSettings &contextSettings, bool isOpenGL) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            HD_LOG_ERROR("Failed to initialize SDL2. Error:\n%s", SDL_GetError());
        }

        uint32_t sdlFlags = SDL_WINDOW_SHOWN | getSDLWindowFlagsFromWindowFlags(flags);
        if (isOpenGL) {
            sdlFlags |= SDL_WINDOW_OPENGL;
            if (contextSettings.isDebug) {
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
            }
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, static_cast<int>(contextSettings.majorVersion));
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, static_cast<int>(contextSettings.minorVersion));
            if (contextSettings.isCoreProfile) {
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            }
            else {
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
            }
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, static_cast<int>(contextSettings.depthBits));
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, static_cast<int>(contextSettings.stencilBits));
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, contextSettings.msaaSamples > 0);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, static_cast<int>(contextSettings.msaaSamples));
        }

        this->flags = flags;
        this->window = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            size.x, size.y, sdlFlags);
        if (!this->window) {
            HD_LOG_ERROR("Failed to create window. Error:\n%s", SDL_GetError());
        }

        if (isOpenGL) {
            this->contextSettings = contextSettings;
            this->glContext = SDL_GL_CreateContext(this->window);
            if (!this->glContext) {
                HD_LOG_ERROR("Failed to create OpenGL context. Error:\n%s", SDL_GetError());
            }
        }

        SDL_Event resizeEvent;
        resizeEvent.type = SDL_WINDOWEVENT;
        resizeEvent.window.event = SDL_WINDOWEVENT_RESIZED;
        resizeEvent.window.data1 = size.x;
        resizeEvent.window.data2 = size.y;
        SDL_PushEvent(&resizeEvent);
    }
};

OpenGLContextSettings::OpenGLContextSettings() {
    this->majorVersion = 4;
    this->minorVersion = 5;
    this->depthBits = 24;
    this->stencilBits = 0;
    this->msaaSamples = 1;
    this->isCoreProfile = false;
#ifdef HD_BUILDMODE_DEBUG
    this->isDebug = true;
#else
    this->isDebug = false;
#endif
}

OpenGLContextSettings::OpenGLContextSettings(uint32_t majorVersion, uint32_t minorVersion, uint32_t depthBits,
    uint32_t stencilBits, uint32_t msaaSamples,
    bool isCoreProfile, bool isDebug) {
    this->majorVersion = majorVersion;
    this->minorVersion = minorVersion;
    this->depthBits = depthBits;
    this->stencilBits = stencilBits;
    this->msaaSamples = msaaSamples;
    this->isCoreProfile = isCoreProfile;
    this->isDebug = isDebug;
}

Window::Window() : impl(new Impl()) {
}

Window::Window(const std::string &title, const glm::ivec2 &size, WindowFlags flags) : Window() {
    create(title, size, flags);
}

Window::Window(const std::string &title, const glm::ivec2 &size, WindowFlags flags,
    const OpenGLContextSettings &contextSettings) : Window() {
    create(title, size, flags, contextSettings);
}

Window::~Window() {
    destroy();
}

void Window::create(const std::string &title, const glm::ivec2 &size, WindowFlags flags) {
    destroy();
    impl->create(title, size, flags, OpenGLContextSettings(), false);
}

void Window::create(const std::string &title, const glm::ivec2 &size, WindowFlags flags,
    const OpenGLContextSettings &contextSettings) {
    destroy();
    impl->create(title, size, flags, contextSettings, true);
}

void Window::destroy() {
    if (impl->texture) {
        SDL_DestroyTexture(impl->texture);
    }
    if (impl->renderer) {
        SDL_DestroyRenderer(impl->renderer);
    }
    if (impl->glContext) {
        SDL_GL_DeleteContext(impl->glContext);
    }
    SDL_DestroyWindow(impl->window);
    SDL_Quit();
    *impl = Impl();
}

bool Window::processEvent(WindowEvent &e) {
    SDL_Event sdlEvent = { 0 };
    if (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
            case SDL_QUIT: {
                e.type = WindowEventType::Close;
                return true;
            }
            case SDL_WINDOWEVENT: {
                switch (sdlEvent.window.event) {
                    case SDL_WINDOWEVENT_RESIZED: {
                        e.type = WindowEventType::Resize;
                        e.resize.width = sdlEvent.window.data1;
                        e.resize.height = sdlEvent.window.data2;
                        impl->isNeedSurfaceResize = true;
                        return true;
                    }
                    case SDL_WINDOWEVENT_FOCUS_LOST: {
                        e.type = WindowEventType::FocusLost;
                        impl->isFocused = false;
                        return true;
                    }
                    case SDL_WINDOWEVENT_FOCUS_GAINED: {
                        e.type = WindowEventType::FocusGained;
                        impl->isFocused = true;
                        return true;
                    }
                }
                break;
            }
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                e.type = WindowEventType::Key;
                e.key.state = sdlEvent.type == SDL_KEYDOWN ? KeyState::Pressed : KeyState::Released;
                e.key.code = getKeyCodeFromSDLScanCode(sdlEvent.key.keysym.scancode);
                const uint8_t *keys = SDL_GetKeyboardState(nullptr);
                e.key.alt = keys[SDL_SCANCODE_LALT] || keys[SDL_SCANCODE_RALT];
                e.key.control = keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL];
                e.key.shift = keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT];
                e.key.system = keys[SDL_SCANCODE_LGUI] || keys[SDL_SCANCODE_RGUI];
                return isFocused();
            }
            case SDL_MOUSEWHEEL: {
                e.type = WindowEventType::MouseWheel;
                if (sdlEvent.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
                    e.mouseWheel.x = -sdlEvent.wheel.x;
                    e.mouseWheel.y = -sdlEvent.wheel.y;
                }
                else {
                    e.mouseWheel.x = sdlEvent.wheel.x;
                    e.mouseWheel.y = sdlEvent.wheel.y;
                }
                uint32_t mouseState = SDL_GetMouseState(nullptr, nullptr);
                e.mouseWheel.leftButton   = HD_FLAG_EXIST(mouseState, SDL_BUTTON(SDL_BUTTON_LEFT  ));
                e.mouseWheel.middleButton = HD_FLAG_EXIST(mouseState, SDL_BUTTON(SDL_BUTTON_MIDDLE));
                e.mouseWheel.rightButton  = HD_FLAG_EXIST(mouseState, SDL_BUTTON(SDL_BUTTON_RIGHT ));
                e.mouseWheel.xButton1     = HD_FLAG_EXIST(mouseState, SDL_BUTTON(SDL_BUTTON_X1    ));
                e.mouseWheel.xButton2     = HD_FLAG_EXIST(mouseState, SDL_BUTTON(SDL_BUTTON_X2    ));
                const uint8_t *keys = SDL_GetKeyboardState(nullptr);
                e.mouseWheel.control = keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL];
                e.mouseWheel.shift = keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT];
                return isFocused();
            }
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN: {
                e.type = WindowEventType::MouseButton;
                e.mouseButton.state = sdlEvent.type == SDL_MOUSEBUTTONDOWN ? KeyState::Pressed : KeyState::Released;
                switch (sdlEvent.button.button) {
                    case SDL_BUTTON_LEFT: {
                        e.mouseButton.btn = MouseButton::Left;
                        break;
                    }
                    case SDL_BUTTON_MIDDLE: {
                        e.mouseButton.btn = MouseButton::Middle;
                        break;
                    }
                    case SDL_BUTTON_RIGHT: {
                        e.mouseButton.btn = MouseButton::Right;
                        break;
                    }
                    case SDL_BUTTON_X1: {
                        e.mouseButton.btn = MouseButton::X1;
                        break;
                    }
                    case SDL_BUTTON_X2: {
                        e.mouseButton.btn = MouseButton::X2;
                        break;
                    }
                }
                e.mouseButton.x = sdlEvent.button.x;
                e.mouseButton.y = sdlEvent.button.y;
                return isFocused();
            }
            case SDL_MOUSEMOTION: {
                e.type = WindowEventType::MouseMove;
                e.mouseMove.x = sdlEvent.motion.x;
                e.mouseMove.y = sdlEvent.motion.y;
                e.mouseMove.deltaX = sdlEvent.motion.xrel;
                e.mouseMove.deltaY = sdlEvent.motion.yrel;
                return isFocused();
            }
        }
    }
    return false;
}

void Window::swapBuffers() {
    if (impl->glContext) {
        SDL_GL_SwapWindow(impl->window);
    }
    else if (impl->renderer && !impl->isSurfaceLocked) {
        SDL_RenderCopy(impl->renderer, impl->texture, nullptr, nullptr);
        SDL_RenderPresent(impl->renderer);
    }
}

void Window::close() {
    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
}

void Window::activate() {
    if (impl->glContext) {
        if (SDL_GL_MakeCurrent(impl->window, impl->glContext) < 0) {
            HD_LOG_ERROR("Failed to set OpenGL context as current. Errors: %s", SDL_GetError());
        }
    }
}

void *Window::lockSurface() {
    if (!impl->renderer) {
        if (impl->glContext) {
            HD_LOG_ERROR("Failed to get surface from OpenGL window");
        }
        impl->renderer = SDL_CreateRenderer(impl->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        HD_ASSERT(impl->renderer);
    }
    if (impl->isNeedSurfaceResize) {
        if (impl->texture) {
            SDL_DestroyTexture(impl->texture);
        }
        impl->texture = SDL_CreateTexture(impl->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, getSize().x, getSize().y);
        HD_ASSERT(impl->texture);
        impl->isNeedSurfaceResize = false;
    }

    if (impl->isSurfaceLocked) {
        HD_LOG_ERROR("Failed to lock window surface. The surface already locked");
    }

    void *surface;
    int pitch;
    int result = SDL_LockTexture(impl->texture, nullptr, &surface, &pitch);
    if (result != 0) {
        HD_LOG_ERROR("Failed to lock window surface");
    }
    impl->isSurfaceLocked = true;
    return surface;
}

void Window::unlockSurface() {
    if (!impl->isSurfaceLocked) {
        HD_LOG_ERROR("Failed to unlock window surface. The surface wasn't locked");
    }
    SDL_UnlockTexture(impl->texture);
    impl->isSurfaceLocked = false;
}

void Window::setTitle(const std::string &title) {
    SDL_SetWindowTitle(impl->window, title.data());
}

void Window::setPosition(const glm::ivec2 &pos) {
    SDL_SetWindowPosition(impl->window, pos.x, pos.y);
}

void Window::setSize(const glm::ivec2 &size) {
    SDL_SetWindowSize(impl->window, size.x, size.y);
}

void Window::setCursorPosition(const glm::ivec2 &pos) {
    SDL_WarpMouseInWindow(impl->window, pos.x, pos.y);
}

void Window::setCursorVisible(bool visible) {
    SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
}

void Window::setVSyncState(bool vsync) {
    SDL_GL_SetSwapInterval(vsync ? 1 : 0);
}

std::string Window::getTitle() const {
    return SDL_GetWindowTitle(impl->window);
}

glm::ivec2 Window::getPosition() const {
    glm::ivec2 v;
    SDL_GetWindowPosition(impl->window, &v.x, &v.y);
    return v;
}

glm::ivec2 Window::getSize() const {
    glm::ivec2 v;
    SDL_GetWindowSize(impl->window, &v.x, &v.y);
    return v;
}

glm::ivec2 Window::getCenter() const {
    return getSize() / 2;
}

WindowFlags Window::getFlags() const {
    return impl->flags;
}

void *Window::getNativeWindowHandle() const {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    if (!SDL_GetWindowWMInfo(impl->window, &info)) {
        HD_LOG_ERROR("Failed to get native window handle. Error:\n%s", SDL_GetError());
    }
#if defined(HD_PLATFORM_UNIX)
    return reinterpret_cast<void*>(info.info.x11.window);
#elif defined(HD_PLATFORM_WIN)
    return info.info.win.window;
#endif
}

bool Window::isFocused() const {
    return impl->isFocused;
}

glm::ivec2 Window::getCursorPosition() const {
    glm::ivec2 v;
    SDL_GetMouseState(&v.x, &v.y);
    return v;
}

bool Window::isCursorVisible() const {
    return SDL_ShowCursor(SDL_QUERY);
}

bool Window::isKeyDown(KeyCode code) const {
    return isFocused() && SDL_GetKeyboardState(nullptr)[gKeyCodes[static_cast<size_t>(code)]];
}

bool Window::isKeyDown(MouseButton button) const {
    uint32_t mouseState = SDL_GetMouseState(nullptr, nullptr);
    return isFocused() && HD_FLAG_EXIST(mouseState, SDL_BUTTON(gMouseButtons[static_cast<size_t>(button)]));
}

bool Window::getVSyncState() const {
    return SDL_GL_GetSwapInterval();
}

const OpenGLContextSettings &Window::getOpenGLContextSettings() const {
    return impl->contextSettings;
}

void *Window::getNativeGLContextHandle() const {
    return impl->glContext;
}

SDL_WindowFlags getSDLWindowFlagsFromWindowFlags(WindowFlags flags) {
    uint32_t sdlFlags = 0;
    if (HD_FLAG_EXIST(flags, WindowFlags::Fullscreen)) {
        sdlFlags |= SDL_WINDOW_FULLSCREEN;
}
    if (HD_FLAG_EXIST(flags, WindowFlags::Hidden)) {
        sdlFlags |= SDL_WINDOW_HIDDEN;
    }
    if (HD_FLAG_EXIST(flags, WindowFlags::Borderless)) {
        sdlFlags |= SDL_WINDOW_BORDERLESS;
    }
    if (HD_FLAG_EXIST(flags, WindowFlags::Resizable)) {
        sdlFlags |= SDL_WINDOW_RESIZABLE;
    }
    if (HD_FLAG_EXIST(flags, WindowFlags::Minimized)) {
        sdlFlags |= SDL_WINDOW_MINIMIZED;
    }
    if (HD_FLAG_EXIST(flags, WindowFlags::Maximized)) {
        sdlFlags |= SDL_WINDOW_MAXIMIZED;
    }
    if (HD_FLAG_EXIST(flags, WindowFlags::InputGrabbed)) {
        sdlFlags |= SDL_WINDOW_INPUT_GRABBED;
    }
    if (HD_FLAG_EXIST(flags, WindowFlags::AllowHighDPI)) {
        sdlFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
    }
    return static_cast<SDL_WindowFlags>(sdlFlags);
}

KeyCode getKeyCodeFromSDLScanCode(SDL_Scancode key) {
    switch (key) {
        case SDL_SCANCODE_A:
            return KeyCode::A;
        case SDL_SCANCODE_B:
            return KeyCode::B;
        case SDL_SCANCODE_C:
            return KeyCode::C;
        case SDL_SCANCODE_D:
            return KeyCode::D;
        case SDL_SCANCODE_E:
            return KeyCode::E;
        case SDL_SCANCODE_F:
            return KeyCode::F;
        case SDL_SCANCODE_G:
            return KeyCode::G;
        case SDL_SCANCODE_H:
            return KeyCode::H;
        case SDL_SCANCODE_I:
            return KeyCode::I;
        case SDL_SCANCODE_J:
            return KeyCode::J;
        case SDL_SCANCODE_K:
            return KeyCode::K;
        case SDL_SCANCODE_L:
            return KeyCode::L;
        case SDL_SCANCODE_M:
            return KeyCode::M;
        case SDL_SCANCODE_N:
            return KeyCode::N;
        case SDL_SCANCODE_O:
            return KeyCode::O;
        case SDL_SCANCODE_P:
            return KeyCode::P;
        case SDL_SCANCODE_Q:
            return KeyCode::Q;
        case SDL_SCANCODE_R:
            return KeyCode::R;
        case SDL_SCANCODE_S:
            return KeyCode::S;
        case SDL_SCANCODE_T:
            return KeyCode::T;
        case SDL_SCANCODE_U:
            return KeyCode::U;
        case SDL_SCANCODE_V:
            return KeyCode::V;
        case SDL_SCANCODE_W:
            return KeyCode::W;
        case SDL_SCANCODE_X:
            return KeyCode::X;
        case SDL_SCANCODE_Y:
            return KeyCode::Y;
        case SDL_SCANCODE_Z:
            return KeyCode::Z;
        case SDL_SCANCODE_0:
            return KeyCode::Num0;
        case SDL_SCANCODE_1:
            return KeyCode::Num1;
        case SDL_SCANCODE_2:
            return KeyCode::Num2;
        case SDL_SCANCODE_3:
            return KeyCode::Num3;
        case SDL_SCANCODE_4:
            return KeyCode::Num4;
        case SDL_SCANCODE_5:
            return KeyCode::Num5;
        case SDL_SCANCODE_6:
            return KeyCode::Num6;
        case SDL_SCANCODE_7:
            return KeyCode::Num7;
        case SDL_SCANCODE_8:
            return KeyCode::Num8;
        case SDL_SCANCODE_9:
            return KeyCode::Num9;
        case SDL_SCANCODE_RETURN:
            return KeyCode::Return;
        case SDL_SCANCODE_ESCAPE:
            return KeyCode::Escape;
        case SDL_SCANCODE_BACKSPACE:
            return KeyCode::BackSpace;
        case SDL_SCANCODE_TAB:
            return KeyCode::Tab;
        case SDL_SCANCODE_SPACE:
            return KeyCode::Space;
        case SDL_SCANCODE_F1:
            return KeyCode::F1;
        case SDL_SCANCODE_F2:
            return KeyCode::F2;
        case SDL_SCANCODE_F3:
            return KeyCode::F3;
        case SDL_SCANCODE_F4:
            return KeyCode::F4;
        case SDL_SCANCODE_F5:
            return KeyCode::F5;
        case SDL_SCANCODE_F6:
            return KeyCode::F6;
        case SDL_SCANCODE_F7:
            return KeyCode::F7;
        case SDL_SCANCODE_F8:
            return KeyCode::F8;
        case SDL_SCANCODE_F9:
            return KeyCode::F9;
        case SDL_SCANCODE_F10:
            return KeyCode::F10;
        case SDL_SCANCODE_F11:
            return KeyCode::F11;
        case SDL_SCANCODE_F12:
            return KeyCode::F12;
        case SDL_SCANCODE_PAUSE:
            return KeyCode::Pause;
        case SDL_SCANCODE_INSERT:
            return KeyCode::Insert;
        case SDL_SCANCODE_HOME:
            return KeyCode::Home;
        case SDL_SCANCODE_PAGEUP:
            return KeyCode::PageUp;
        case SDL_SCANCODE_PAGEDOWN:
            return KeyCode::PageDown;
        case SDL_SCANCODE_DELETE:
            return KeyCode::Delete;
        case SDL_SCANCODE_END:
            return KeyCode::End;
        case SDL_SCANCODE_LEFT:
            return KeyCode::Left;
        case SDL_SCANCODE_RIGHT:
            return KeyCode::Right;
        case SDL_SCANCODE_UP:
            return KeyCode::Up;
        case SDL_SCANCODE_DOWN:
            return KeyCode::Down;
        case SDL_SCANCODE_KP_0:
            return KeyCode::Numpad0;
        case SDL_SCANCODE_KP_1:
            return KeyCode::Numpad1;
        case SDL_SCANCODE_KP_2:
            return KeyCode::Numpad2;
        case SDL_SCANCODE_KP_3:
            return KeyCode::Numpad3;
        case SDL_SCANCODE_KP_4:
            return KeyCode::Numpad4;
        case SDL_SCANCODE_KP_5:
            return KeyCode::Numpad5;
        case SDL_SCANCODE_KP_6:
            return KeyCode::Numpad6;
        case SDL_SCANCODE_KP_7:
            return KeyCode::Numpad7;
        case SDL_SCANCODE_KP_8:
            return KeyCode::Numpad8;
        case SDL_SCANCODE_KP_9:
            return KeyCode::Numpad9;
        case SDL_SCANCODE_LCTRL:
            return KeyCode::LControl;
        case SDL_SCANCODE_LSHIFT:
            return KeyCode::LShift;
        case SDL_SCANCODE_LALT:
            return KeyCode::LAlt;
        case SDL_SCANCODE_LGUI:
            return KeyCode::LSystem;
        case SDL_SCANCODE_RCTRL:
            return KeyCode::RControl;
        case SDL_SCANCODE_RSHIFT:
            return KeyCode::RShift;
        case SDL_SCANCODE_RALT:
            return KeyCode::RAlt;
        case SDL_SCANCODE_RGUI:
            return KeyCode::RSystem;
        default:
            return KeyCode::Unknown;
    }
}

}
