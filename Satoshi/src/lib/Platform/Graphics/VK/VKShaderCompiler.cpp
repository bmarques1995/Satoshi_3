#include "Platform/Graphics/VK/VKShaderCompiler.hpp"
#include "Satoshi/Utils/FileHandler.hpp"
#include "Satoshi/Core/Console.hpp"

std::unordered_map<Satoshi::SHADER_KIND, shaderc_shader_kind> Satoshi::VKShaderCompiler::s_MappedShaderKinds = 
{
	{SHADER_KIND::SHADER_KIND_VERTEX, shaderc_shader_kind::shaderc_vertex_shader},
	{SHADER_KIND::SHADER_KIND_PIXEL, shaderc_shader_kind::shaderc_fragment_shader},
	{SHADER_KIND::SHADER_KIND_GEOMETRY, shaderc_shader_kind::shaderc_geometry_shader},
	{SHADER_KIND::SHADER_KIND_DOMAIN, shaderc_shader_kind::shaderc_glsl_tess_evaluation_shader},
	{SHADER_KIND::SHADER_KIND_HULL, shaderc_shader_kind::shaderc_glsl_tess_control_shader},
	{SHADER_KIND::SHADER_KIND_COMPUTE, shaderc_shader_kind::shaderc_compute_shader}
};

Satoshi::VKShaderCompiler::VKShaderCompiler(bool useHLSL) :
	m_UseHLSL(useHLSL)
{
}

Satoshi::VKShaderCompiler::~VKShaderCompiler()
{
}

Satoshi::COMPILE_ERRORS Satoshi::VKShaderCompiler::CompileFromFile(std::string_view baseShaderPath, std::string_view entrypoint, SHADER_KIND shaderKind, SHADER_VERSION shaderVersion, uint32_t optimizationLevel, uint32_t extraFlags)
{
	BuildInputFile(shaderKind, baseShaderPath);
	std::string shaderSource;
	if (!FileHandler::ReadTextFile(s_InputFile, &shaderSource))
		return COMPILE_ERRORS::COMPILER_FILE_NOT_FOUND;

	shaderc::CompileOptions options;
	shaderc::Compiler compiler;
	options.SetSourceLanguage(m_UseHLSL ? shaderc_source_language::shaderc_source_language_hlsl : shaderc_source_language::shaderc_source_language_glsl);
	options.SetOptimizationLevel((shaderc_optimization_level)optimizationLevel);

	shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(shaderSource, s_MappedShaderKinds[shaderKind], s_InputFile.c_str(), options);
	if (result.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		Console::CoreError("{0}", result.GetErrorMessage());
		return COMPILE_ERRORS::COMPILER_ERROR_SOURCE_CODE;
	}

	BuildOutputFile(shaderKind, shaderVersion, baseShaderPath);

	FileHandler::WriteBinFile(s_OutputFile, (std::byte*)(result.begin()), (result.end() - result.begin())*sizeof(uint32_t));

	return COMPILE_ERRORS::COMPILER_COMPILE_SUCCESS;
}

const std::unordered_map<Satoshi::SHADER_KIND, std::string_view> Satoshi::VKShaderCompiler::GetShaderKindStringMap()
{
    return s_ShaderKindStringMap;
}

const std::unordered_map<Satoshi::SHADER_VERSION, std::string_view> Satoshi::VKShaderCompiler::GetShaderVersion()
{
    return s_ShaderVersion;
}

void Satoshi::VKShaderCompiler::BuildInputFile(SHADER_KIND shaderKind, std::string_view baseShaderPath)
{
	std::stringstream inputFile;
	auto shaderKindStringMap = s_ShaderKindStringMap;
	auto shaderVersions = s_ShaderVersion;
	inputFile << baseShaderPath;
	inputFile << "." << shaderKindStringMap[shaderKind];
	inputFile << (m_UseHLSL ? ".hlsl" : ".glsl");
	s_InputFile = inputFile.str();
}

void Satoshi::VKShaderCompiler::BuildOutputFile(SHADER_KIND shaderKind, SHADER_VERSION hlslVersion, std::string_view baseShaderPath)
{
	std::stringstream outputFile;
	auto shaderKindStringMap = s_ShaderKindStringMap;
	auto shaderVersions = s_ShaderVersion;
	outputFile << baseShaderPath;
	outputFile << "." << shaderKindStringMap[shaderKind] << shaderVersions[hlslVersion];
	outputFile << ".spv";
	s_OutputFile = outputFile.str();
}
