#ifndef CORE_HPP
#define CORE_HPP

#ifdef ST_PLATFORM_WINDOWS
	#ifdef ST_BUILD_DLL
		#define SATOSHI_API __declspec(dllexport)
	#else
		#define SATOSHI_API __declspec(dllimport)
	#endif
#endif // ST_PLATFORM_WINDOWS


#endif //CORE_HPP
