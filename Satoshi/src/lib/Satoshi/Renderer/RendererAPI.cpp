#include "Satoshi/Renderer/RendererAPI.hpp"

std::unordered_map<std::string_view, Satoshi::GRAPHICS_API> Satoshi::RendererAPI::s_APINamesMapper =
{
    {"GL4", GRAPHICS_API::GL4},
    {"OpenGL", GRAPHICS_API::GL4},
    {"OpenGL 4", GRAPHICS_API::GL4},
    {"GL3", GRAPHICS_API::GL4},
    {"OpenGL 3", GRAPHICS_API::GL4},
    {"DirectX 11", GRAPHICS_API::D3D11},
    {"Direct3D 11", GRAPHICS_API::D3D11},
    {"DX11", GRAPHICS_API::D3D11},
    {"D3D11", GRAPHICS_API::D3D11},
    {"Vulkan", GRAPHICS_API::VK},
    {"VK", GRAPHICS_API::VK},
    {"Vk", GRAPHICS_API::VK},
    {"DirectX 12", GRAPHICS_API::D3D12},
    {"Direct3D 12", GRAPHICS_API::D3D12},
    {"DX12", GRAPHICS_API::D3D12},
    {"D3D12", GRAPHICS_API::D3D12}
};

Satoshi::GRAPHICS_API Satoshi::RendererAPI::MatchAPIByName(std::string_view name)
{
    return s_APINamesMapper[name];
}
