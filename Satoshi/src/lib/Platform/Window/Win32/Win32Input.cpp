#ifdef ST_PLATFORM_WINDOWS

#include "Platform/Window/Win32/Win32Input.hpp"
#include "Platform/Window/Win32/Win32InputMapper.hpp"

Satoshi::Win32Input::Win32Input(StWindowHandle window) : 
	m_Window(window)
{
}

Satoshi::Win32Input::~Win32Input()
{
}

bool Satoshi::Win32Input::IsKeyPressedImpl(uint32_t keycode)
{
	uint16_t keyState = GetKeyState(Win32InputMapper::MapInput(keycode));
	return keyState & BIT(15);
}

bool Satoshi::Win32Input::IsMouseButtonPressedImpl(uint32_t button)
{
	uint16_t buttonState = GetKeyState(Win32InputMapper::MapInput(button));
	return buttonState & BIT(15);
}

float Satoshi::Win32Input::GetMouseXImpl()
{
	return GetMousePosImpl().first;
}

float Satoshi::Win32Input::GetMouseYImpl()
{
	return GetMousePosImpl().second;
}

std::pair<float, float> Satoshi::Win32Input::GetMousePosImpl()
{
	RECT windowPos;
	POINT mousePos;
	GetWindowRect(m_Window, &windowPos);
	GetCursorPos(&mousePos);
	return std::pair<float, float>((float)(mousePos.x - windowPos.left), (float)(mousePos.y - windowPos.top));
}

#endif
