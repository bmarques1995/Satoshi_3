#include "Platform/Graphics/D3D12/D3D12ShaderManager.hpp"
#include "Satoshi/Utils/FileHandler.hpp"

Satoshi::D3D12ShaderManager::D3D12ShaderManager() :
	m_Compiler(new D3D12ShaderCompiler())
{
}

Satoshi::D3D12ShaderManager::~D3D12ShaderManager()
{
	delete m_Compiler;
}

void Satoshi::D3D12ShaderManager::CompileShaderFamily(const ShaderGroup& shaderGroup)
{
	bool debugMode;
	uint32_t compileFlags;
#ifdef _DEBUG
	compileFlags = D3DCOMPILE_DEBUG;
	debugMode = true;
#else
	compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
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

const std::string& Satoshi::D3D12ShaderManager::BuildBlobFilename(std::string_view baseShaderPath, GRAPHICS_SHADER_KIND shaderKind, SHADER_VERSION shaderVersion)
{
	std::stringstream outputFile;
	auto shaderKindStringMap = m_Compiler->GetShaderKindStringMap();
	auto shaderVersions = m_Compiler->GetShaderVersion();
	outputFile << baseShaderPath;
	outputFile << "." << shaderKindStringMap[shaderKind] << shaderVersions[shaderVersion];
	outputFile << ".dxbc";
	s_Filename = outputFile.str();
	return s_Filename;
}
