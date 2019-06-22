#include "hdWindow.hpp"
#include "../Core/hdStringUtils.hpp"
#include <Windows.h>

namespace hd {

static const uint32_t gKeyCodesToWinKeys[] = {
    0,
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    VK_RETURN, VK_ESCAPE, VK_BACK, VK_TAB, VK_SPACE,
    VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12,
    VK_PAUSE, VK_INSERT, VK_HOME, VK_PRIOR, VK_NEXT, VK_DELETE, VK_END, VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,
    VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9,
    VK_LCONTROL, VK_RCONTROL, VK_LSHIFT, VK_RSHIFT, VK_LMENU, VK_RMENU, VK_LWIN, VK_RWIN
};

static const uint32_t gMouseButtonCodesToWinKeys[] = {
    VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2
};

struct Window::Impl {
    Impl() {
        this->hwnd = nullptr;
    }

    HWND hwnd;
    std::string wndClassName;
    std::wstring wndClassNameWide;
    std::vector<WindowEvent> events;
    bool isFocused;
};

LPARAM CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
KeyCode getKeyCodeFromWinKey(uint32_t key);

Window::Window() : impl(std::make_unique<Impl>()) {

}

Window::Window(const std::string &title, uint32_t w, uint32_t h) : Window() {
    create(title, w, h);
}

Window::~Window() {
    destroy();
}

void Window::create(const std::string &title, uint32_t w, uint32_t h) {
    impl->wndClassName = title;
    impl->wndClassNameWide = hd::StringUtils::toWideString(title);

    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = impl->wndClassNameWide.data();
    wc.lpszMenuName = nullptr;
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (FAILED(RegisterClassEx(&wc))) {
        HD_LOG_ERROR("Failed to register WinAPI window class '%s'", title.data());
    }

    RECT rc;
    ZeroMemory(&rc, sizeof(RECT));
    rc.left = GetSystemMetrics(SM_CXSCREEN) / 2 - w / 2;
    rc.top = GetSystemMetrics(SM_CYSCREEN) / 2 - h / 2;
    rc.right = rc.left + w;
    rc.bottom = rc.top + h;
    if (!AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false)) {
        HD_LOG_ERROR("Failed to adjust WinAPI window rect 'left = %d, right = %d, bottom = %d, top = %d'", rc.left, rc.right, rc.bottom, rc.top);
    }

    auto x = rc.left;
    auto y = rc.top;
    auto newW = rc.right - rc.left;
    auto newH = rc.bottom - rc.top;
    impl->hwnd = CreateWindow(impl->wndClassNameWide.data(), impl->wndClassNameWide.data(), WS_OVERLAPPEDWINDOW, x, y, newW, newH, nullptr, nullptr, wc.hInstance, 0);
    if (!impl->hwnd) {
        HD_LOG_ERROR("Failed to create WinAPI window");
    }
    SetWindowLongPtr(impl->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(impl.get()));

    ShowWindow(impl->hwnd, SW_SHOW);
    UpdateWindow(impl->hwnd);
}

void Window::destroy() {
    DestroyWindow(impl->hwnd);
    UnregisterClass(impl->wndClassNameWide.data(), GetModuleHandle(nullptr));
}

bool Window::processEvent(WindowEvent &e) {
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (!impl->events.empty())
    {
        e = impl->events.back();
        impl->events.pop_back();
        return true;
    }
    else
        return false;
}

void Window::close() {
    PostMessage(impl->hwnd, WM_CLOSE, 0, 0);
}

void Window::setCursorPos(int x, int y) {
    SetCursorPos(x + getPosX(), y + getPosY());
}

int Window::getCursorPosX() const {
    POINT p;
    GetCursorPos(&p);
    return p.x - getPosX();
}

int Window::getCursorPosY() const {
    POINT p;
    GetCursorPos(&p);
    return p.y - getPosY();
}

void Window::setCursorVisible(bool visible) {
    if (visible) {
        while(ShowCursor(true) <= 0);
    }
    else {
        while(ShowCursor(false) >= 0);
    }
}

bool Window::isCursorVisible() const {
    CURSORINFO info;
    GetCursorInfo(&info);
    return info.flags == CURSOR_SHOWING;
}

bool Window::isKeyDown(KeyCode code) const {
    return GetAsyncKeyState(gKeyCodesToWinKeys[static_cast<int>(code)]);
}

bool Window::isMouseButtonDown(MouseButton button) const {
    return GetAsyncKeyState(gMouseButtonCodesToWinKeys[static_cast<int>(button)]);
}

bool Window::isOpened() const {
    return impl->hwnd != nullptr;
}

bool Window::isFocused() const {
    return impl->isFocused;
}

void Window::setTitle(const std::string &title) {
    SetWindowText(impl->hwnd, hd::StringUtils::toWideString(title).data());
}

std::string Window::getTitle() const {
    std::wstring title;
    title.resize(GetWindowTextLength(impl->hwnd));
    GetWindowText(impl->hwnd, title.data(), title.size());
    return hd::StringUtils::fromWideString(title);
}

void Window::setPos(int x, int y) {
    SetWindowPos(impl->hwnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

int Window::getPosX() const {
    RECT rc;
    ZeroMemory(&rc, sizeof(RECT));
    GetWindowRect(impl->hwnd, &rc);
    return rc.left;
}

int Window::getPosY() const {
    RECT rc;
    ZeroMemory(&rc, sizeof(RECT));
    GetWindowRect(impl->hwnd, &rc);
    return rc.top;
}

void Window::setSize(uint32_t w, uint32_t h) {
    SetWindowPos(impl->hwnd, nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE);
}

int Window::getSizeX() const {
    RECT rc;
    ZeroMemory(&rc, sizeof(RECT));
    ::GetWindowRect(impl->hwnd, &rc);
    return rc.right - rc.left;
}

int Window::getSizeY() const {
    RECT rc;
    ZeroMemory(&rc, sizeof(RECT));
    ::GetWindowRect(impl->hwnd, &rc);
    return rc.bottom - rc.top;
}

int Window::getCenterX() const {
    return static_cast<int>(static_cast<float>(getSizeX()) / 2.0f);
}

int Window::getCenterY() const {
    return static_cast<int>(static_cast<float>(getSizeY()) / 2.0f);
}

void *Window::getOSWindowHandle() const {
    return impl->hwnd;
}

LPARAM CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto impl = reinterpret_cast<Window::Impl*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    WindowEvent e;
    switch (msg) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        case WM_CLOSE: {
            e.type = WindowEventType::Close;
            impl->events.push_back(e);
            break;
        }
        case WM_SIZE: {
            e.type = WindowEventType::Resize;
            e.resize.width = LOWORD(lparam);
            e.resize.height = HIWORD(lparam);
            impl->events.push_back(e);
            break;
        }
        case WM_KILLFOCUS: {
            e.type = WindowEventType::FocusLost;
            impl->isFocused = false;
            impl->events.push_back(e);
            break;
        }
        case WM_SETFOCUS: {
            e.type = WindowEventType::FocusGained;
            impl->isFocused = true;
            impl->events.push_back(e);
            break;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
            e.type = WindowEventType::Key;
            e.key.state = KeyState::Pressed;
            e.key.code = getKeyCodeFromWinKey(wparam);
            e.key.alt = HIWORD(GetAsyncKeyState(VK_MENU)) != 0;
            e.key.control = HIWORD(GetAsyncKeyState(VK_CONTROL)) != 0;
            e.key.shift = HIWORD(GetAsyncKeyState(VK_SHIFT)) != 0;
            e.key.system = HIWORD(GetAsyncKeyState(VK_LWIN)) != 0 || HIWORD(GetAsyncKeyState(VK_RWIN)) != 0;
            if (impl->isFocused) {
                impl->events.push_back(e);
            }
            break;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            e.type = WindowEventType::Key;
            e.key.state = KeyState::Released;
            e.key.code = getKeyCodeFromWinKey(wparam);
            e.key.alt = HIWORD(GetAsyncKeyState(VK_MENU)) != 0;
            e.key.control = HIWORD(GetAsyncKeyState(VK_CONTROL)) != 0;
            e.key.shift = HIWORD(GetAsyncKeyState(VK_SHIFT)) != 0;
            e.key.system = HIWORD(GetAsyncKeyState(VK_LWIN)) != 0 || HIWORD(GetAsyncKeyState(VK_RWIN)) != 0;
            if (impl->isFocused) {
                impl->events.push_back(e);
            }
            break;
        }
        case WM_MOUSEWHEEL: {
            e.type = WindowEventType::MouseWheel;
            //e.mouseWheel.delta = static_cast<short>(HIWORD(wparam)) / 120;
            POINT pos = { static_cast<short>(LOWORD(lparam)), static_cast<short>(HIWORD(lparam)) };
            ScreenToClient(impl->hwnd, &pos);
            e.mouseWheel.x = pos.x;
            e.mouseWheel.y = pos.y;
            e.mouseWheel.control = (LOWORD(wparam) & MK_CONTROL) == MK_CONTROL;
            e.mouseWheel.leftButton = (LOWORD(wparam) & MK_LBUTTON) == MK_LBUTTON;
            e.mouseWheel.middleButton = (LOWORD(wparam) & MK_MBUTTON) == MK_MBUTTON;
            e.mouseWheel.rightButton = (LOWORD(wparam) & MK_RBUTTON) == MK_RBUTTON;
            e.mouseWheel.shift = (LOWORD(wparam) & MK_SHIFT) == MK_SHIFT;
            e.mouseWheel.xButton1 = (LOWORD(wparam) & MK_XBUTTON1) == MK_XBUTTON1;
            e.mouseWheel.xButton2 = (LOWORD(wparam) & MK_XBUTTON2) == MK_XBUTTON2;
            if (impl->isFocused) {
                impl->events.push_back(e);
            }
            break;
        }
        case WM_LBUTTONDOWN: {
            e.type = WindowEventType::MouseButton;
            e.mouseButton.state = KeyState::Pressed;
            e.mouseButton.btn = MouseButton::Left;
            e.mouseButton.x = static_cast<short>(LOWORD(lparam));
            e.mouseButton.y = static_cast<short>(HIWORD(lparam));
            if (impl->isFocused) {
                impl->events.push_back(e);
            }
            break;
        }
        case WM_LBUTTONUP: {
            e.type = WindowEventType::MouseButton;
            e.mouseButton.state = KeyState::Released;
            e.mouseButton.btn = MouseButton::Left;
            e.mouseButton.x = static_cast<short>(LOWORD(lparam));
            e.mouseButton.y = static_cast<short>(HIWORD(lparam));
            if (impl->isFocused) {
                impl->events.push_back(e);
            }
            break;
        }
        case WM_MBUTTONDOWN: {
            e.type = WindowEventType::MouseButton;
            e.mouseButton.state = KeyState::Pressed;
            e.mouseButton.btn = MouseButton::Middle;
            e.mouseButton.x = static_cast<short>(LOWORD(lparam));
            e.mouseButton.y = static_cast<short>(HIWORD(lparam));
            if (impl->isFocused) {
                impl->events.push_back(e);
            }
            break;
        }
        case WM_MBUTTONUP: {
            e.type = WindowEventType::MouseButton;
            e.mouseButton.state = KeyState::Released;
            e.mouseButton.btn = MouseButton::Middle;
            e.mouseButton.x = static_cast<short>(LOWORD(lparam));
            e.mouseButton.y = static_cast<short>(HIWORD(lparam));
            if (impl->isFocused) {
                impl->events.push_back(e);
            }
            break;
        }
        case WM_RBUTTONDOWN: {
            e.type = WindowEventType::MouseButton;
            e.mouseButton.state = KeyState::Pressed;
            e.mouseButton.btn = MouseButton::Right;
            e.mouseButton.x = static_cast<short>(LOWORD(lparam));
            e.mouseButton.y = static_cast<short>(HIWORD(lparam));
            if (impl->isFocused) {
                impl->events.push_back(e);
            }
            break;
        }
        case WM_RBUTTONUP: {
            e.type = WindowEventType::MouseButton;
            e.mouseButton.state = KeyState::Released;
            e.mouseButton.btn = MouseButton::Right;
            e.mouseButton.x = static_cast<short>(LOWORD(lparam));
            e.mouseButton.y = static_cast<short>(HIWORD(lparam));
            if (impl->isFocused) {
                impl->events.push_back(e);
            }
            break;
        }
        case WM_XBUTTONDOWN: {
            e.type = WindowEventType::MouseButton;
            e.mouseButton.state = KeyState::Pressed;
            e.mouseButton.btn = HIWORD(wparam) == XBUTTON1 ? MouseButton::X1 : MouseButton::X2;
            e.mouseButton.x = static_cast<short>(LOWORD(lparam));
            e.mouseButton.y = static_cast<short>(HIWORD(lparam));
            if (impl->isFocused) {
                impl->events.push_back(e);
            }
            break;
        }
        case WM_XBUTTONUP: {
            e.type = WindowEventType::MouseButton;
            e.mouseButton.state = KeyState::Released;
            e.mouseButton.btn = HIWORD(wparam) == XBUTTON1 ? MouseButton::X1 : MouseButton::X2;
            e.mouseButton.x = static_cast<short>(LOWORD(lparam));
            e.mouseButton.y = static_cast<short>(HIWORD(lparam));
            if (impl->isFocused) {
                impl->events.push_back(e);
            }
            break;
        }
        case WM_MOUSEMOVE: {
            /*e.type = WindowEventType::MouseMove;
            e.mouseMove.x = static_cast<short>(LOWORD(lparam));
            e.mouseMove.y = static_cast<short>(HIWORD(lparam));*/
            static auto lastX = 0, lastY = 0;
            static auto first = true;
            if (first) {
                first = false;
                lastX = static_cast<short>(LOWORD(lparam));
                lastY = static_cast<short>(HIWORD(lparam));
            }
            e.type = WindowEventType::MouseMove;
            e.mouseMove.x = static_cast<short>(LOWORD(lparam));
            e.mouseMove.y = static_cast<short>(HIWORD(lparam));
            e.mouseMove.deltaX = lastX - static_cast<short>(LOWORD(lparam));
            e.mouseMove.deltaY = lastY - static_cast<short>(HIWORD(lparam));
            lastX = static_cast<short>(LOWORD(lparam));
            lastY = static_cast<short>(HIWORD(lparam));
            if (impl->isFocused) {
                impl->events.push_back(e);
            }
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

KeyCode getKeyCodeFromWinKey(uint32_t key) {
    switch (key) {
        case 'A':
            return KeyCode::A;
        case 'B':
            return KeyCode::B;
        case 'C':
            return KeyCode::C;
        case 'D':
            return KeyCode::D;
        case 'E':
            return KeyCode::E;
        case 'F':
            return KeyCode::F;
        case 'G':
            return KeyCode::G;
        case 'H':
            return KeyCode::H;
        case 'I':
            return KeyCode::I;
        case 'J':
            return KeyCode::J;
        case 'K':
            return KeyCode::K;
        case 'L':
            return KeyCode::L;
        case 'M':
            return KeyCode::M;
        case 'N':
            return KeyCode::N;
        case 'O':
            return KeyCode::O;
        case 'P':
            return KeyCode::P;
        case 'Q':
            return KeyCode::Q;
        case 'R':
            return KeyCode::R;
        case 'S':
            return KeyCode::S;
        case 'T':
            return KeyCode::T;
        case 'U':
            return KeyCode::U;
        case 'V':
            return KeyCode::V;
        case 'W':
            return KeyCode::W;
        case 'X':
            return KeyCode::X;
        case 'Y':
            return KeyCode::Y;
        case 'Z':
            return KeyCode::Z;
        case '0':
            return KeyCode::Num0;
        case '1':
            return KeyCode::Num1;
        case '2':
            return KeyCode::Num2;
        case '3':
            return KeyCode::Num3;
        case '4':
            return KeyCode::Num4;
        case '5':
            return KeyCode::Num5;
        case '6':
            return KeyCode::Num6;
        case '7':
            return KeyCode::Num7;
        case '8':
            return KeyCode::Num8;
        case '9':
            return KeyCode::Num9;
        case VK_RETURN:
            return KeyCode::Return;
        case VK_ESCAPE:
            return KeyCode::Escape;
        case VK_BACK:
            return KeyCode::BackSpace;
        case VK_TAB:
            return KeyCode::Tab;
        case VK_SPACE:
            return KeyCode::Space;
        case VK_F1:
            return KeyCode::F1;
        case VK_F2:
            return KeyCode::F2;
        case VK_F3:
            return KeyCode::F3;
        case VK_F4:
            return KeyCode::F4;
        case VK_F5:
            return KeyCode::F5;
        case VK_F6:
            return KeyCode::F6;
        case VK_F7:
            return KeyCode::F7;
        case VK_F8:
            return KeyCode::F8;
        case VK_F9:
            return KeyCode::F9;
        case VK_F10:
            return KeyCode::F10;
        case VK_F11:
            return KeyCode::F11;
        case VK_F12:
            return KeyCode::F12;
        case VK_PAUSE:
            return KeyCode::Pause;
        case VK_INSERT:
            return KeyCode::Insert;
        case VK_HOME:
            return KeyCode::Home;
        case VK_PRIOR:
            return KeyCode::PageUp;
        case VK_NEXT:
            return KeyCode::PageDown;
        case VK_DELETE:
            return KeyCode::Delete;
        case VK_END:
            return KeyCode::End;
        case VK_LEFT:
            return KeyCode::Left;
        case VK_RIGHT:
            return KeyCode::Right;
        case VK_UP:
            return KeyCode::Up;
        case VK_DOWN:
            return KeyCode::Down;
        case VK_NUMPAD0:
            return KeyCode::Numpad0;
        case VK_NUMPAD1:
            return KeyCode::Numpad1;
        case VK_NUMPAD2:
            return KeyCode::Numpad2;
        case VK_NUMPAD3:
            return KeyCode::Numpad3;
        case VK_NUMPAD4:
            return KeyCode::Numpad4;
        case VK_NUMPAD5:
            return KeyCode::Numpad5;
        case VK_NUMPAD6:
            return KeyCode::Numpad6;
        case VK_NUMPAD7:
            return KeyCode::Numpad7;
        case VK_NUMPAD8:
            return KeyCode::Numpad8;
        case VK_NUMPAD9:
            return KeyCode::Numpad9;
        case VK_LCONTROL:
            return KeyCode::LControl;
        case VK_LSHIFT:
            return KeyCode::LShift;
        case VK_LMENU:
            return KeyCode::LAlt;
        case VK_LWIN:
            return KeyCode::LSystem;
        case VK_RCONTROL:
            return KeyCode::RControl;
        case VK_RSHIFT:
            return KeyCode::RShift;
        case VK_RMENU:
            return KeyCode::RAlt;
        case VK_RWIN:
            return KeyCode::RSystem;
        default:
            return KeyCode::Unknown;
    }
}

}
