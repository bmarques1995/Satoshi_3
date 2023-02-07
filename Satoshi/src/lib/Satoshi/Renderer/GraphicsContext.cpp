#include "Satoshi/Renderer/GraphicsContext.hpp"
#include "Platform/Graphics/GL4/GL4Context.hpp"
#include "Platform/Graphics/VK/VKContext.hpp"
#ifdef ST_PLATFORM_WINDOWS
#include "Platform/Graphics/D3D11/D3D11Context.hpp"
#include "Platform/Graphics/D3D12/D3D12Context.hpp"
#endif // ST_PLATFORM_WINDOWS

Satoshi::GraphicsContext* Satoshi::GraphicsContext::Create(StWindowHandle window, uint32_t width, uint32_t height, GRAPHICS_API api)
{
	switch (api)
	{
	case Satoshi::GRAPHICS_API::GL4:
		return new GL4Context(window, width, height);
	case Satoshi::GRAPHICS_API::VK:
		return new VKContext(window);
#ifdef ST_PLATFORM_WINDOWS
	case Satoshi::GRAPHICS_API::D3D11:
		return new D3D11Context(window, width, height);
	case Satoshi::GRAPHICS_API::D3D12:
		return new D3D12Context(window, width, height);
#endif // ST_PLATFORM_WINDOWS
	default:
		return nullptr;
	}
}
