#include "Satoshi/Window/Input.hpp"
#ifdef ST_PLATFORM_WINDOWS
#include "Platform/Window/Win32/Win32Input.hpp"
#else
#error Window API not implemented
#endif // ST_PLATFORM_WINDOWS

Satoshi::Input* Satoshi::Input::s_Instance;

void Satoshi::Input::Start(StWindowHandle window)
{
#ifdef ST_PLATFORM_WINDOWS
	s_Instance = new Win32Input(window);
#endif // ST_PLATFORM_WINDOWS
}

void Satoshi::Input::End()
{
	delete s_Instance;
}
