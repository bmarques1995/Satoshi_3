#ifdef ST_PLATFORM_WINDOWS

#include "Platform/Window/Win32/Win32Window.hpp"
#include "Satoshi/Events/ApplicationEvent.hpp"
#include "Satoshi/Events/KeyEvent.hpp"
#include "Satoshi/Events/MouseEvent.hpp"

Satoshi::Win32Window::Win32Window(const WindowProps& props)
{
    m_WindowData.Title = std::wstring(props.Title.begin(), props.Title.end());
    m_WindowData.Width = props.Width;
    m_WindowData.Height = props.Height;
    m_ShouldClose = false;
    //window
    HINSTANCE hInstance = GetModuleHandleW(NULL);
    LPWSTR cmdLine = GetCommandLineW();
    DWORD cmdShow = 10;
    RECT windowDimensions = { 0, 0, (LONG)m_WindowData.Width, (LONG)m_WindowData.Height };

    CreateWindowClass(&hInstance);
    RegisterClassExW(&m_WindowClass);

    DWORD windowedFlags = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION | WS_MAXIMIZEBOX | WS_THICKFRAME;

    AdjustDimensions(&windowDimensions, windowedFlags);

    m_WindowHandle = CreateWindowExW
    (
        0,
        m_WindowData.Title.c_str(),
        m_WindowData.Title.c_str(),
        windowedFlags,
        100,
        100,
        windowDimensions.right - windowDimensions.left,
        windowDimensions.bottom - windowDimensions.top,
        nullptr,
        nullptr,
        m_WindowClass.hInstance,
        nullptr
    );

    auto test = GetLastError();
    assert(GetLastError() == 0);

    RegisterCallbacks();
    SetWindowLongPtrW(m_WindowHandle, 0, (LONG_PTR)&m_WindowData);

    ShowWindow(m_WindowHandle, cmdShow);
    UpdateWindow(m_WindowHandle);
}

Satoshi::Win32Window::~Win32Window()
{
    DestroyWindow(m_WindowHandle);
    UnregisterClassW(m_WindowClass.lpszClassName, m_WindowClass.hInstance);
}

void Satoshi::Win32Window::OnUpdate()
{
    if (::PeekMessage(&m_MSG, NULL, 0, 0, PM_REMOVE))
    {
        ::TranslateMessage(&m_MSG);
        ::DispatchMessage(&m_MSG);
    }

}

uint32_t Satoshi::Win32Window::GetWidth() const
{
    return m_WindowData.Width;
}

uint32_t Satoshi::Win32Window::GetHeight() const
{
    return m_WindowData.Height;
}

bool Satoshi::Win32Window::ShouldClose() const
{
    return m_ShouldClose;
}

void Satoshi::Win32Window::SetCloseState(bool value)
{
    m_ShouldClose = value;
}

void Satoshi::Win32Window::SetEventCallback(const EventCallbackFn& callback)
{
    m_WindowData.EventCallback = callback;
}

std::any Satoshi::Win32Window::GetNativeWindow() const
{
    return m_WindowHandle;
}

void Satoshi::Win32Window::CreateWindowClass(HINSTANCE* instance)
{
    m_WindowClass = { 0 };
    m_WindowClass.hInstance = *instance;
    m_WindowClass.cbSize = sizeof(m_WindowClass);
    m_WindowClass.cbClsExtra = 0;
    m_WindowClass.cbWndExtra = sizeof(void*);
    m_WindowClass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    m_WindowClass.hIcon = nullptr;
    m_WindowClass.hCursor = LoadCursor(m_WindowClass.hInstance, IDC_ARROW);
    m_WindowClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    m_WindowClass.lpszClassName = m_WindowData.Title.c_str();
    m_WindowClass.lpszMenuName = nullptr;

    m_WindowClass.lpfnWndProc = [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
    {

        WindowData* m_Data = (WindowData*)GetWindowLongPtr(hWnd, 0);

        if (msg == WM_NCCREATE || m_Data == nullptr)
        {
            return DefWindowProcW(hWnd, msg, wParam, lParam);
        }

        if (m_Data->CallbackResolver.find(msg) != m_Data->CallbackResolver.end())
        {
            m_Data->CallbackResolver[msg](hWnd, wParam, lParam, m_Data);
        }

        return DefWindowProcW(hWnd, msg, wParam, lParam);
    };
}

void Satoshi::Win32Window::AdjustDimensions(LPRECT originalDimensions, DWORD flags)
{
    AdjustWindowRectEx(originalDimensions, flags, 0, 0);
}

void Satoshi::Win32Window::RegisterCallbacks()
{
    m_WindowData.CallbackResolver[WM_SIZING] = 
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        RECT clientArea;
        GetClientRect(hWnd, &clientArea);
        WindowResizeEvent e((unsigned)(clientArea.right - clientArea.left), (unsigned)(clientArea.bottom - clientArea.top));
        m_Data->Width = e.GetWidth();
        m_Data->Height = e.GetHeight();
        m_Data->EventCallback(e);
    };

    m_WindowData.CallbackResolver[WM_QUIT] = m_WindowData.CallbackResolver[WM_CLOSE] = 
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        WindowCloseEvent e;
        m_Data->EventCallback(e);
    };

    m_WindowData.CallbackResolver[WM_KEYDOWN] =
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        uint16_t repeated = (bool)(HIWORD(lParam) & KF_REPEAT);
        KeyPressedEvent event(static_cast<uint32_t>(wParam), repeated);
        m_Data->EventCallback(event);
    };

    m_WindowData.CallbackResolver[WM_KEYUP] =
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        KeyReleasedEvent event(static_cast<uint32_t>(wParam));
        m_Data->EventCallback(event);
    };

    m_WindowData.CallbackResolver[WM_CHAR] =
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        KeyTypedEvent event(static_cast<uint32_t>(wParam));
        m_Data->EventCallback(event);
    };

    m_WindowData.CallbackResolver[WM_LBUTTONDOWN] =
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        MouseButtonPressedEvent event(0);
        m_Data->EventCallback(event);
    };
    
    m_WindowData.CallbackResolver[WM_MBUTTONDOWN] =
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        MouseButtonPressedEvent event(2);
        m_Data->EventCallback(event);
    };
    
    m_WindowData.CallbackResolver[WM_RBUTTONDOWN] =
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        MouseButtonPressedEvent event(1);
        m_Data->EventCallback(event);
    };

    m_WindowData.CallbackResolver[WM_XBUTTONDOWN] = 
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        MouseButtonPressedEvent event((unsigned)wParam);
        m_Data->EventCallback(event);
    };

    m_WindowData.CallbackResolver[WM_LBUTTONUP] =
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        MouseButtonReleasedEvent event(0);
        m_Data->EventCallback(event);
    };

    m_WindowData.CallbackResolver[WM_MBUTTONUP] = 
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        MouseButtonReleasedEvent event(2);
        m_Data->EventCallback(event);
    };

    m_WindowData.CallbackResolver[WM_RBUTTONUP] =
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        MouseButtonReleasedEvent event(1);
        m_Data->EventCallback(event);
    };

    m_WindowData.CallbackResolver[WM_XBUTTONUP] = 
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        MouseButtonReleasedEvent event((unsigned)wParam);
        m_Data->EventCallback(event);
    };

    m_WindowData.CallbackResolver[WM_MOUSEHWHEEL] = 
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        short delta = GET_WHEEL_DELTA_WPARAM(wParam);
        MouseScrolledEvent event((float)(delta / 120), 0);
        m_Data->EventCallback(event);
    };

    m_WindowData.CallbackResolver[WM_MOUSEWHEEL] = 
        [](HWND hWnd, WPARAM wParam, LPARAM lParam, WindowData* m_Data) -> void
    {
        short delta = GET_WHEEL_DELTA_WPARAM(wParam);
        MouseScrolledEvent event(0, (float)(delta / 120));
        m_Data->EventCallback(event);
    };
}

#endif // ST_PLATFORM_WINDOWS
