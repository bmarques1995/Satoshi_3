#include "Satoshi/Renderer/ShaderManager.hpp"

std::string Satoshi::ShaderManager::s_Filename;

#include "Platform/Graphics/GL4/GL4ShaderManager.hpp"
#include "Platform/Graphics/VK/VKShaderManager.hpp"
#ifdef ST_PLATFORM_WINDOWS
#include "Platform/Graphics/D3D11/D3D11ShaderManager.hpp"
#include "Platform/Graphics/D3D12/D3D12ShaderManager.hpp"
#endif // ST_PLATFORM_WINDOWS

Satoshi::ShaderManager* Satoshi::ShaderManager::Create(GRAPHICS_API api, bool useHLSL)
{
	switch (api)
	{
	case Satoshi::GRAPHICS_API::GL4:
		return new GL4ShaderManager();
	case Satoshi::GRAPHICS_API::VK:
		return new VKShaderManager(useHLSL);
#ifdef ST_PLATFORM_WINDOWS
	case Satoshi::GRAPHICS_API::D3D11:
		return new D3D11ShaderManager();
	case Satoshi::GRAPHICS_API::D3D12:
		return new D3D12ShaderManager();
#endif // ST_PLATFORM_WINDOWS
	default:
		return nullptr;
	}
}
