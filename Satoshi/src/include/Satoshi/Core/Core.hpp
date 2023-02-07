#ifndef CORE_HPP
#define CORE_HPP

#ifdef ST_PLATFORM_WINDOWS
#include <windows.h>
#endif // ST_PLATFORM_WINDOWS


#ifdef ST_PLATFORM_WINDOWS
	#ifdef ST_BUILD_DLL
		#define SATOSHI_API __declspec(dllexport)
	#else
		#define SATOSHI_API __declspec(dllimport)
	#endif
#endif // ST_PLATFORM_WINDOWS

#ifdef ST_PLATFORM_WINDOWS
typedef HWND StWindowHandle;
#else
#error API not implemented
#endif

#define BIT(x) (1 << x)

#endif //CORE_HPP
