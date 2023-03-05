#include "Satoshi/Renderer/ShaderCompiler.hpp"

const std::unordered_map<Satoshi::GRAPHICS_SHADER_KIND, std::string_view> Satoshi::ShaderCompiler::s_ShaderKindStringMap =
{
	{GRAPHICS_SHADER_KIND::SHADER_KIND_VERTEX, "vs"},
	{GRAPHICS_SHADER_KIND::SHADER_KIND_PIXEL, "ps"},
	{GRAPHICS_SHADER_KIND::SHADER_KIND_GEOMETRY, "gs"},
	{GRAPHICS_SHADER_KIND::SHADER_KIND_HULL, "hs"},
	{GRAPHICS_SHADER_KIND::SHADER_KIND_DOMAIN, "ds"}
};

const std::unordered_map<Satoshi::SHADER_VERSION, std::string_view> Satoshi::ShaderCompiler::s_ShaderVersion =
{
	{SHADER_VERSION::GL_GLSL, ""},
	{SHADER_VERSION::VK_GLSL, ""},
	{SHADER_VERSION::VK_HLSL, ""},
	{SHADER_VERSION::HLSL_5_0, "_5_0"},
};

