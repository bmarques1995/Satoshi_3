#ifndef SHADER_COMPILER_HPP
#define SHADER_COMPILER_HPP

#include <stpch.hpp>
#include "Satoshi/Renderer/RendererAPI.hpp"

namespace Satoshi
{
	enum class COMPILE_ERRORS
	{
		COMPILER_COMPILE_SUCCESS = 0,
		COMPILER_FILE_NOT_FOUND,
		COMPILER_ERROR_SOURCE_CODE

	};

	enum class GRAPHICS_SHADER_KIND
	{
		SHADER_KIND_VERTEX = 0,
		SHADER_KIND_PIXEL,
		SHADER_KIND_GEOMETRY,
		SHADER_KIND_HULL,
		SHADER_KIND_DOMAIN
	};

	enum class SHADER_VERSION
	{
		GL_GLSL,
		VK_GLSL,
		VK_HLSL,
		HLSL_5_0
	};

	class ShaderCompiler
	{
	public:
		virtual COMPILE_ERRORS CompileFromFile(std::string_view baseShaderPath, std::string_view entrypoint, GRAPHICS_SHADER_KIND shaderKind, SHADER_VERSION shaderVersion, uint32_t optimizationLevel, uint32_t extraFlags = 0) = 0;
		virtual const std::unordered_map<GRAPHICS_SHADER_KIND, std::string_view> GetShaderKindStringMap() = 0;
		virtual const std::unordered_map<SHADER_VERSION, std::string_view> GetShaderVersion() = 0;

	protected:
		static const std::unordered_map<GRAPHICS_SHADER_KIND, std::string_view> s_ShaderKindStringMap;
		static const std::unordered_map<SHADER_VERSION, std::string_view> s_ShaderVersion;
	};
}


#endif //SHADER_COMPILER_HPP
