#include "Satoshi/Window/Window.hpp"
#ifdef ST_PLATFORM_WINDOWS
#include "Platform/Window/Win32/Win32Window.hpp"
#endif

Satoshi::Window* Satoshi::Window::Create(const WindowProps& props)
{
#ifdef ST_PLATFORM_WINDOWS
    return new Win32Window(props);
#endif
}
