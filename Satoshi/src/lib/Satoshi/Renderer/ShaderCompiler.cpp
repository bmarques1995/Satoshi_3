#include "Satoshi/Renderer/ShaderCompiler.hpp"

const std::unordered_map<Satoshi::SHADER_KIND, std::string_view> Satoshi::ShaderCompiler::s_ShaderKindStringMap =
{
	{SHADER_KIND::SHADER_KIND_VERTEX, "vs"},
	{SHADER_KIND::SHADER_KIND_PIXEL, "ps"},
	{SHADER_KIND::SHADER_KIND_GEOMETRY, "gs"},
	{SHADER_KIND::SHADER_KIND_HULL, "hs"},
	{SHADER_KIND::SHADER_KIND_DOMAIN, "ds"},
	{SHADER_KIND::SHADER_KIND_COMPUTE, "cs"},
};

const std::unordered_map<Satoshi::SHADER_VERSION, std::string_view> Satoshi::ShaderCompiler::s_ShaderVersion =
{
	{SHADER_VERSION::GL_GLSL, ""},
	{SHADER_VERSION::VK_GLSL, ""},
	{SHADER_VERSION::VK_HLSL, ""},
	{SHADER_VERSION::HLSL_5_1, "_5_1"},
};

