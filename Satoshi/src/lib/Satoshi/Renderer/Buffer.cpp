#include "Satoshi/Renderer/Buffer.hpp"
//#include "Platform/Graphics/GL4/GL4Buffer.hpp"
//#include "Platform/Graphics/VK/VKBuffer.hpp"
#ifdef ST_PLATFORM_WINDOWS
#include "Platform/Graphics/D3D11/D3D11Buffer.hpp"
//#include "Platform/Graphics/D3D12/D3D12Buffer.hpp"
#endif // ST_PLATFORM_WINDOWS

Satoshi::VertexBuffer* Satoshi::VertexBuffer::Create(const void* data, size_t size, uint32_t stride, GRAPHICS_API api)
{
	switch (api)
	{
		/*case Satoshi::GRAPHICS_API::GL4:
			return new GL4Shader(window, width, height);
		case Satoshi::GRAPHICS_API::VK:
			return new VKShader(window);*/
#ifdef ST_PLATFORM_WINDOWS
	case Satoshi::GRAPHICS_API::D3D11:
		return new D3D11VertexBuffer(data, size, stride);
		/*case Satoshi::GRAPHICS_API::D3D12:
			return new D3D12Shader(window, width, height);*/
#endif // ST_PLATFORM_WINDOWS
	default:
		return nullptr;
	}
}

Satoshi::IndexBuffer* Satoshi::IndexBuffer::Create(const void* data, size_t count, GRAPHICS_API api)
{
	switch (api)
	{
		/*case Satoshi::GRAPHICS_API::GL4:
			return new GL4Shader(window, width, height);
		case Satoshi::GRAPHICS_API::VK:
			return new VKShader(window);*/
#ifdef ST_PLATFORM_WINDOWS
	case Satoshi::GRAPHICS_API::D3D11:
		return new D3D11IndexBuffer(data, count);
		/*case Satoshi::GRAPHICS_API::D3D12:
			return new D3D12Shader(window, width, height);*/
#endif // ST_PLATFORM_WINDOWS
	default:
		return nullptr;
	}
}
