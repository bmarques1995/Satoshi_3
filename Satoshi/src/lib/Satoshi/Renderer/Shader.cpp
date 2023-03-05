#include "Satoshi/Renderer/Shader.hpp"
#include "Platform/Graphics/GL4/GL4Shader.hpp"
#include "Platform/Graphics/VK/VKShader.hpp"
#ifdef ST_PLATFORM_WINDOWS
#include "Platform/Graphics/D3D11/D3D11Shader.hpp"
#include "Platform/Graphics/D3D12/D3D12Shader.hpp"
#endif // ST_PLATFORM_WINDOWS

Satoshi::Shader* Satoshi::Shader::Create(ShaderGroup shaderGroup, const std::initializer_list<BufferElement>& elements, GRAPHICS_API api)
{
	switch (api)
	{
	case Satoshi::GRAPHICS_API::GL4:
		return new GL4Shader(shaderGroup, elements);
	case Satoshi::GRAPHICS_API::VK:
		return new VKShader(shaderGroup, elements);
#ifdef ST_PLATFORM_WINDOWS
	case Satoshi::GRAPHICS_API::D3D11:
		return new D3D11Shader(shaderGroup, elements);
	case Satoshi::GRAPHICS_API::D3D12:
		return new D3D12Shader(shaderGroup, elements);
#endif // ST_PLATFORM_WINDOWS
	default:
		return nullptr;
	}
}
