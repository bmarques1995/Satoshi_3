#ifndef ENTRYPOINT_HPP
#define ENTRYPOINT_HPP

#ifdef ST_PLATFORM_WINDOWS

#include <windows.h>
#include <Satoshi.hpp>
#include "Satoshi/Events/KeyEvent.hpp"

INT APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	Satoshi::KeyPressedEvent e(65);
	Satoshi::Console::Init();
	Satoshi::Console::Log("Hello Logger");
	Satoshi::Console::Log(e.ToString());
	Satoshi::Console::End();
	while (true);
	return 0;
}

#endif

#endif //ENTRYPOINT_HPP
