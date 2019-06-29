#include "hdWindow.hpp"
#include "hdOSSpecificInfo.hpp"
#include "../../3rd/include/SDL2/SDL.h"

namespace hd {

static const int gKeyCodes[] =
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

uint32_t getSDLWindowFlagsFromWindowFlags(WindowFlags flags);
KeyCode getKeyCodeFromSDLScanCode(int16_t key);

struct _SDLVideoSystem {
    _SDLVideoSystem() {
        if (SDL_VideoInit(nullptr) < 0) {
            HD_LOG_ERROR("Failed to initialize SDL video system. Errors: %s", SDL_GetError());
        }
    }
    ~_SDLVideoSystem() {
        SDL_VideoQuit();
    }
} _gSDLVideoSystem;

struct Window::Impl {
    SDL_Window *window = nullptr;
    SDL_Surface *windowSurface = nullptr;
    SDL_GLContext openglContext = nullptr;
    WindowFlags flags = WindowFlags::None;
    OpenGLContextSettings contextSettings;
    bool isFocused = true;

    void createWindow(const std::string &title, uint32_t w, uint32_t h, WindowFlags flags, bool forOpenGL) {
        auto sdlFlags = SDL_WINDOW_SHOWN | getSDLWindowFlagsFromWindowFlags(flags);
        if (forOpenGL) {
            sdlFlags |= SDL_WINDOW_OPENGL;
        }
        this->flags = flags;
        this->window = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, sdlFlags);
        if (!this->window) {
            HD_LOG_ERROR("Failed to create '%s' window %dx%d. Errors: %s", title.data(), w, h, SDL_GetError());
        }
        this->windowSurface = SDL_GetWindowSurface(this->window); // Channel order in memory: BGRA
        if (this->windowSurface->format->format != SDL_PIXELFORMAT_RGB888) {
            HD_LOG_ERROR("Unsupported window pixel format '%s'", SDL_GetPixelFormatName(this->windowSurface->format->format));
        }

        SDL_Event resizeEvent;
        resizeEvent.type = SDL_WINDOWEVENT;
        resizeEvent.window.event = SDL_WINDOWEVENT_RESIZED;
        resizeEvent.window.data1 = w;
        resizeEvent.window.data2 = h;
        SDL_PushEvent(&resizeEvent);
    }

    void createContext(const OpenGLContextSettings &contextSettings) {
        this->contextSettings = contextSettings;
        this->openglContext = SDL_GL_CreateContext(this->window);
        if (!this->openglContext) {
            HD_LOG_ERROR("Failed to create %sOpenGL %d.%d context with %d depth bits, %d stencil bits and %d MSAA samples. Errors: %s",
                contextSettings.isCoreProfile ? "core profile " : "", contextSettings.majorVersion, contextSettings.minorVersion,
                contextSettings.depthBits, contextSettings.stencilBits, contextSettings.msaaSamples, SDL_GetError());
        }
    }
};

OpenGLContextSettings::OpenGLContextSettings() {
    this->majorVersion = 4;
    this->minorVersion = 5;
    this->depthBits = 24;
    this->stencilBits = 0;
    this->msaaSamples = 1;
    this->isCoreProfile = true;
#ifdef HD_BUILDMODE_DEBUG
    this->isDebug = true;
#else
    this->isDebug = false;
#endif
}

OpenGLContextSettings::OpenGLContextSettings(uint32_t majorVersion, uint32_t minorVersion, uint32_t depthBits, uint32_t stencilBits, uint32_t msaaSamples, bool isCoreProfile, bool isDebug) {
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

Window::Window(const std::string &title, uint32_t w, uint32_t h, WindowFlags flags) : Window() {
    create(title, w, h, flags);
}

Window::Window(const std::string &title, uint32_t w, uint32_t h, WindowFlags flags, const OpenGLContextSettings &contextSettings) : Window() {
    create(title, w, h, flags, contextSettings);
}

Window::~Window() {
    destroy();
}

void Window::create(const std::string &title, uint32_t w, uint32_t h, WindowFlags flags) {
    destroy();
    impl->createWindow(title, w, h, flags, false);
    SDL_ShowWindow(impl->window);
}

void Window::create(const std::string &title, uint32_t w, uint32_t h, WindowFlags flags, const OpenGLContextSettings &contextSettings) {
    destroy();
    if (contextSettings.isDebug) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, static_cast<int>(contextSettings.majorVersion));
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, static_cast<int>(contextSettings.minorVersion));
    if (contextSettings.isCoreProfile) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    }
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, static_cast<int>(contextSettings.depthBits));
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, static_cast<int>(contextSettings.stencilBits));
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, contextSettings.msaaSamples > 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, static_cast<int>(contextSettings.msaaSamples));
    impl->createWindow(title, w, h, flags, true);
    impl->createContext(contextSettings);
    SDL_ShowWindow(impl->window);
    setVSyncState(false);
}

void Window::destroy() {
    if (isOpened()) {
        if (impl->openglContext) {
            SDL_GL_DeleteContext(impl->openglContext);
        }
        SDL_DestroyWindow(impl->window);
        *impl = Impl();
    }
}

void Window::activate() {
    if (isOpened() && impl->openglContext) {
        if (SDL_GL_MakeCurrent(impl->window, impl->openglContext) < 0) {
            HD_LOG_ERROR("Failed to activate window %s. Errors: %s", SDL_GetWindowTitle(impl->window), SDL_GetError());
        }
    }
}

bool Window::processEvent(WindowEvent &e) {
    SDL_Event sdlEvent = { 0 };
    if (SDL_PollEvent(&sdlEvent)) {
        uint32_t mouseState;
        switch (sdlEvent.type) {
            case SDL_QUIT:
                e.type = WindowEventType::Close;
                return true;
            case SDL_WINDOWEVENT:
                switch (sdlEvent.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        e.type = WindowEventType::Resize;
                        e.resize.width = sdlEvent.window.data1;
                        e.resize.height = sdlEvent.window.data2;
                        return true;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        e.type = WindowEventType::FocusLost;
                        impl->isFocused = false;
                        return true;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        e.type = WindowEventType::FocusGained;
                        impl->isFocused = true;
                        return true;
                }
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                e.type = WindowEventType::Key;
                e.key.state = sdlEvent.type == SDL_KEYDOWN ? KeyState::Pressed : KeyState::Released;
                e.key.code = getKeyCodeFromSDLScanCode(sdlEvent.key.keysym.scancode);
                e.key.alt = SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LALT] || SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_RALT];
                e.key.control = SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LCTRL] || SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_RCTRL];
                e.key.shift = SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LSHIFT] || SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_RSHIFT];
                e.key.system = SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LGUI] || SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_RGUI];
                return isFocused();
            case SDL_MOUSEWHEEL:
                e.type = WindowEventType::MouseWheel;
                if (sdlEvent.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
                    e.mouseWheel.x = -sdlEvent.wheel.x;
                    e.mouseWheel.y = -sdlEvent.wheel.y;
                }
                else {
                    e.mouseWheel.x = sdlEvent.wheel.x;
                    e.mouseWheel.y = sdlEvent.wheel.y;
                }
                mouseState = SDL_GetMouseState(nullptr, nullptr);
                e.mouseWheel.leftButton   = HD_FLAG_EXIST(mouseState, SDL_BUTTON(SDL_BUTTON_LEFT  ));
                e.mouseWheel.middleButton = HD_FLAG_EXIST(mouseState, SDL_BUTTON(SDL_BUTTON_MIDDLE));
                e.mouseWheel.rightButton  = HD_FLAG_EXIST(mouseState, SDL_BUTTON(SDL_BUTTON_RIGHT ));
                e.mouseWheel.xButton1     = HD_FLAG_EXIST(mouseState, SDL_BUTTON(SDL_BUTTON_X1    ));
                e.mouseWheel.xButton2     = HD_FLAG_EXIST(mouseState, SDL_BUTTON(SDL_BUTTON_X2    ));
                e.mouseWheel.control = SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LCTRL] || SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_RCTRL];
                e.mouseWheel.shift = SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LSHIFT] || SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_RSHIFT];
                return isFocused();
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
                e.type = WindowEventType::MouseButton;
                e.mouseButton.state = sdlEvent.type == SDL_MOUSEBUTTONDOWN ? KeyState::Pressed : KeyState::Released;
                switch (sdlEvent.button.button) {
                    case SDL_BUTTON_LEFT:
                        e.mouseButton.btn = MouseButton::Left;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        e.mouseButton.btn = MouseButton::Middle;
                        break;
                    case SDL_BUTTON_RIGHT:
                        e.mouseButton.btn = MouseButton::Right;
                        break;
                    case SDL_BUTTON_X1:
                        e.mouseButton.btn = MouseButton::X1;
                        break;
                    case SDL_BUTTON_X2:
                        e.mouseButton.btn = MouseButton::X2;
                        break;
                }
                e.mouseButton.x = sdlEvent.button.x;
                e.mouseButton.y = sdlEvent.button.y;
                return isFocused();
            case SDL_MOUSEMOTION:
                e.type = WindowEventType::MouseMove;
                e.mouseMove.x = sdlEvent.motion.x;
                e.mouseMove.y = sdlEvent.motion.y;
                e.mouseMove.deltaX = sdlEvent.motion.xrel;
                e.mouseMove.deltaY = sdlEvent.motion.yrel;
                return isFocused();
        }
    }
    return false;
}

void Window::swapBuffers() {
    if (isOpened()) {
        if (impl->openglContext) {
            SDL_GL_SwapWindow(impl->window);
        }
        else {
            SDL_UpdateWindowSurface(impl->window);
        }
    }
}

void Window::close() {
    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
}

void Window::setHighPriorityProcess() {
    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);
}

void Window::setVSyncState(bool vsync) {
    SDL_GL_SetSwapInterval(vsync ? 1 : 0);
}

bool Window::getVSyncState() const {
    return SDL_GL_GetSwapInterval();
}

void Window::setCursorPosition(int x, int y) {
    if (isOpened()) {
        SDL_WarpMouseInWindow(impl->window, x, y);
    }
}

int Window::getCursorPositionX() const {
    int x;
    if (isOpened()) {
        SDL_GetMouseState(&x, nullptr);
    }
    return x;
}

int Window::getCursorPositionY() const {
    int y;
    if (isOpened()) {
        SDL_GetMouseState(nullptr, &y);
    }
    return y;
}

void Window::setCursorVisible(bool visible) {
    if (isOpened()) {
        SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
    }
}

bool Window::isCursorVisible() const {
    return isOpened() && SDL_ShowCursor(SDL_QUERY);
}

bool Window::isKeyDown(KeyCode code) const {
    return isOpened() && isFocused() && SDL_GetKeyboardState(nullptr)[gKeyCodes[static_cast<size_t>(code)]];
}

bool Window::isMouseButtonDown(MouseButton button) const {
    return isOpened() && isFocused() && HD_FLAG_EXIST(SDL_GetMouseState(nullptr, nullptr), SDL_BUTTON(gMouseButtons[static_cast<size_t>(button)]));
}

bool Window::isOpened() const {
    return impl->window != nullptr;
}

bool Window::isFocused() const {
    return impl->isFocused;
}

void Window::setTitle(const std::string &title) {
    if (isOpened()) {
        SDL_SetWindowTitle(impl->window, title.data());
    }
}

std::string Window::getTitle() const {
    if (isOpened()) {
        return SDL_GetWindowTitle(impl->window);
    }
    return std::string("");
}

void Window::setPosition(int x, int y) {
    if (isOpened()) {
        SDL_SetWindowPosition(impl->window, x, y);
    }
}

int Window::getPositionX() const {
    int x;
    if (isOpened()) {
        SDL_GetWindowPosition(impl->window, &x, nullptr);
    }
    return x;
}

int Window::getPositionY() const {
    int y;
    if (isOpened()) {
        SDL_GetWindowPosition(impl->window, nullptr, &y);
    }
    return y;
}

void Window::setSize(uint32_t w, uint32_t h) {
    if (isOpened()) {
        SDL_SetWindowSize(impl->window, w, h);
    }
}

int Window::getSizeX() const {
    int x;
    if (isOpened()) {
        SDL_GetWindowSize(impl->window, &x, nullptr);
    }
    return x;
}

int Window::getSizeY() const {
    int y;
    if (isOpened()) {
        SDL_GetWindowSize(impl->window, nullptr, &y);
    }
    return y;
}

int Window::getCenterX() const {
    return getSizeX() / 2;
}

int Window::getCenterY() const {
    return getSizeY() / 2;
}

WindowFlags Window::getFlags() const {
    return impl->flags;
}

bool Window::hasOpenGLContext() const {
    return impl->openglContext != nullptr;
}

const OpenGLContextSettings &Window::getOpenGLContextSettings() const {
    return impl->contextSettings;
}

Color4 *Window::getSurface() const {
    return static_cast<Color4*>(impl->windowSurface->pixels);
}

void *Window::getOSWindowHandle() const {
    return isOpened() ? getSystemWindowHandle(impl->window) : nullptr;
}

void *Window::getOSContextHandle() const {
    return isOpened() ? impl->openglContext : nullptr;
}

uint32_t getSDLWindowFlagsFromWindowFlags(WindowFlags flags) {
    uint32_t sdlFlags = 0;
    if (flags != WindowFlags::None) {
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
    }
    return sdlFlags;
}

KeyCode getKeyCodeFromSDLScanCode(int16_t key) {
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
