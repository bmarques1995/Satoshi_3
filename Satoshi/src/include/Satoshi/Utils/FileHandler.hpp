#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <stpch.hpp>
#include "Satoshi/Core/Core.hpp"

namespace Satoshi
{
	class SATOSHI_API FileHandler
	{
	public:
		static bool ReadTextFile(std::string_view path, std::string* content);
		static bool WriteTextFile(std::string_view path, std::string content);
		static bool ReadBinFile(std::string_view path, std::byte** content);
		static bool WriteBinFile(std::string_view path, std::byte* content, size_t dataSize);

		static bool FileExists(std::string_view path);
	};
}

#endif //FILE_HANDLER_HPP