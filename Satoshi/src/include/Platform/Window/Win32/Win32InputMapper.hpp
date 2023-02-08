#ifdef ST_PLATFORM_WINDOWS

#ifndef WIN32_INPUT_MAPPER_HPP
#define WIN32_INPUT_MAPPER_HPP

#include <stpch.hpp>
#include <Satoshi/Window/InputCodes.hpp>

namespace Satoshi
{
	class Win32InputMapper
	{
	public:
		static uint16_t MapInput(uint16_t code)
		{
			uint16_t key = 0;
			if (s_Codes.find(code) != s_Codes.end())
				key = s_Codes[code];
			return key;
		}
	private:
		static std::unordered_map<uint16_t, uint16_t> s_Codes;
	};
}


#endif //WIN32_INPUT_MAPPER_HPP

#endif // DEBUG