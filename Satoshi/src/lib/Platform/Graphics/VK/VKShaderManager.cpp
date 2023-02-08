#include "Platform/Graphics/VK/VKShaderManager.hpp"
#include "Satoshi/Utils/FileHandler.hpp"

Satoshi::VKShaderManager::VKShaderManager() :
	m_Compiler(new VKShaderCompiler())
{
}

Satoshi::VKShaderManager::~VKShaderManager()
{
}

void Satoshi::VKShaderManager::CompileShaderFamily(const ShaderGroup& shaderGroup)
{
	bool debugMode;
	uint32_t compileFlags;
#ifdef _DEBUG
	compileFlags = shaderc_optimization_level::shaderc_optimization_level_zero;
	debugMode = true;
#else
	compileFlags = shaderc_optimization_level::shaderc_optimization_level_performance;
	debugMode = false;
#endif // _DEBUG

	auto details = shaderGroup.GetShaderDetails();
	bool blobExists = true;

	if (details.size() > 0)
		blobExists = FileHandler::FileExists(BuildBlobFilename(shaderGroup.GetBasePath(), details[0].ShaderKind, shaderGroup.GetShaderVersion()));

	if (debugMode || !blobExists)
	{
		for (size_t i = 0; i < details.size(); i++)
		{
			m_Compiler->CompileFromFile(shaderGroup.GetBasePath(), details[i].Entrypoint, details[i].ShaderKind, shaderGroup.GetShaderVersion(), compileFlags);
		}
	}
}

const std::string& Satoshi::VKShaderManager::BuildBlobFilename(std::string_view baseShaderPath, SHADER_KIND shaderKind, SHADER_VERSION shaderVersion)
{
	std::stringstream outputFile;
	auto shaderKindStringMap = m_Compiler->GetShaderKindStringMap();
	auto shaderVersions = m_Compiler->GetShaderVersion();
	outputFile << baseShaderPath;
	outputFile << "." << shaderKindStringMap[shaderKind] << shaderVersions[shaderVersion];
	outputFile << ".spv";
	s_Filename = outputFile.str();
	return s_Filename;
}
