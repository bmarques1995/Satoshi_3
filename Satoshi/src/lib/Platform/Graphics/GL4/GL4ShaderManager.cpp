#include "Platform/Graphics/GL4/GL4ShaderManager.hpp"

Satoshi::GL4ShaderManager::GL4ShaderManager() :
	m_Compiler(new GL4ShaderCompiler())
{
}

Satoshi::GL4ShaderManager::~GL4ShaderManager()
{
	delete m_Compiler;
}

void Satoshi::GL4ShaderManager::CompileShaderFamily(const ShaderGroup& shaderGroup)
{
}

const std::string& Satoshi::GL4ShaderManager::BuildBlobFilename(std::string_view baseShaderPath, SHADER_KIND shaderKind, SHADER_VERSION shaderVersion)
{
	std::stringstream outputFile;
	auto shaderKindStringMap = m_Compiler->GetShaderKindStringMap();
	auto shaderVersions = m_Compiler->GetShaderVersion();

	outputFile << baseShaderPath;
	outputFile << "." << shaderKindStringMap[shaderKind] << shaderVersions[shaderVersion];
	outputFile << ".glsl";
	s_Filename = outputFile.str();
	return s_Filename;
}

