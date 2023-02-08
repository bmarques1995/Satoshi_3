#ifdef ST_PLATFORM_WINDOWS

#include "Platform/Graphics/D3D11/D3D11ShaderCompiler.hpp"
#include "Satoshi/Utils/FileHandler.hpp"
#include "Satoshi/Core/Console.hpp"

Satoshi::D3D11ShaderCompiler::D3D11ShaderCompiler()
{
}

Satoshi::D3D11ShaderCompiler::~D3D11ShaderCompiler()
{
}

Satoshi::COMPILE_ERRORS Satoshi::D3D11ShaderCompiler::CompileFromFile(std::string_view baseShaderPath, std::string_view entrypoint, SHADER_KIND shaderKind, SHADER_VERSION shaderVersion, uint32_t optimizationLevel, uint32_t extraFlags)
{
	BuildInputFile(shaderKind, baseShaderPath);
	std::string shaderSource;
	if (!FileHandler::ReadTextFile(s_InputFile, &shaderSource))
		return COMPILE_ERRORS::COMPILER_FILE_NOT_FOUND;
	BuildTarget(shaderKind, shaderVersion);

	ID3DBlob* shaderBlob;
	ID3DBlob* errorBlob;

	HRESULT hr = D3DCompile(shaderSource.c_str(), shaderSource.size(), nullptr, nullptr, nullptr, entrypoint.data(), s_Target.c_str(), optimizationLevel, extraFlags, &shaderBlob, &errorBlob);

	if (errorBlob)
	{
		if (errorBlob->GetBufferSize())
		{
			Console::CoreError("{0}", (const char*)errorBlob->GetBufferPointer());
		}
		errorBlob->Release();
		return COMPILE_ERRORS::COMPILER_ERROR_SOURCE_CODE;
	}

	BuildOutputFile(shaderKind, shaderVersion, baseShaderPath);

	FileHandler::WriteBinFile(s_OutputFile, (std::byte*)shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());

	shaderBlob->Release();
	return COMPILE_ERRORS::COMPILER_COMPILE_SUCCESS;
}

const std::unordered_map<Satoshi::SHADER_KIND, std::string_view> Satoshi::D3D11ShaderCompiler::GetShaderKindStringMap()
{
    return s_ShaderKindStringMap;
}

const std::unordered_map<Satoshi::SHADER_VERSION, std::string_view> Satoshi::D3D11ShaderCompiler::GetShaderVersion()
{
    return s_ShaderVersion;
}

void Satoshi::D3D11ShaderCompiler::BuildTarget(SHADER_KIND shaderKind, SHADER_VERSION shaderVersion)
{
	std::stringstream target;
	auto shaderKindStringMap = s_ShaderKindStringMap;
	auto shaderVersions = s_ShaderVersion;
	target << shaderKindStringMap[shaderKind];
	target << shaderVersions[shaderVersion];
	s_Target = target.str();
}

void Satoshi::D3D11ShaderCompiler::BuildInputFile(SHADER_KIND shaderKind, std::string_view baseShaderPath)
{
	std::stringstream inputFile;
	auto shaderKindStringMap = s_ShaderKindStringMap;
	inputFile << baseShaderPath;
	inputFile << "." << shaderKindStringMap[shaderKind];
	inputFile << ".hlsl";
	s_InputFile = inputFile.str();
}

void Satoshi::D3D11ShaderCompiler::BuildOutputFile(SHADER_KIND shaderKind, SHADER_VERSION hlslVersion, std::string_view baseShaderPath)
{
	std::stringstream outputFile;
	auto shaderKindStringMap = s_ShaderKindStringMap;
	auto shaderVersions = s_ShaderVersion;
	outputFile << baseShaderPath;
	outputFile << "." << shaderKindStringMap[shaderKind] << shaderVersions[hlslVersion];
	outputFile << ".dxbc";
	s_OutputFile = outputFile.str();
}

#endif