#include "Satoshi.hpp"

#ifdef ST_PLATFORM_WINDOWS

#include <windows.h>

INT APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	Satoshi::Console::Init();
	Satoshi::Console::Log("Hello Logger");
	Satoshi::Console::End();
	while (true);
	return 0;
}

#endif
