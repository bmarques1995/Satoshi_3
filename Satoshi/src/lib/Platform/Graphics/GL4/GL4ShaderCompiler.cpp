#include "Platform/Graphics/GL4/GL4ShaderCompiler.hpp"

Satoshi::GL4ShaderCompiler::GL4ShaderCompiler()
{
}

Satoshi::GL4ShaderCompiler::~GL4ShaderCompiler()
{
}

Satoshi::COMPILE_ERRORS Satoshi::GL4ShaderCompiler::CompileFromFile(std::string_view baseShaderPath, std::string_view entrypoint, SHADER_KIND shaderKind, SHADER_VERSION hlslVersion, uint32_t optimizationLevel, uint32_t extraFlags)
{
    return COMPILE_ERRORS::COMPILER_COMPILE_SUCCESS;
}

const std::unordered_map<Satoshi::SHADER_KIND, std::string_view> Satoshi::GL4ShaderCompiler::GetShaderKindStringMap()
{
    return s_ShaderKindStringMap;
}

const std::unordered_map<Satoshi::SHADER_VERSION, std::string_view> Satoshi::GL4ShaderCompiler::GetShaderVersion()
{
    return s_ShaderVersion;
}
