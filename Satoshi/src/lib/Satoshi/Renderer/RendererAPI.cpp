#include "Satoshi/Renderer/RendererAPI.hpp"

std::unordered_map<std::string_view, Satoshi::GRAPHICS_API> Satoshi::RendererAPI::s_APINamesMapper =
{
    {"GL4", GRAPHICS_API::GL4},
    {"OPENGL", GRAPHICS_API::GL4},
    {"OPENGL 4", GRAPHICS_API::GL4},
    {"GL3", GRAPHICS_API::GL4},
    {"OPENGL 3", GRAPHICS_API::GL4},
    {"DIRECTX 11", GRAPHICS_API::D3D11},
    {"DIRECT3D 11", GRAPHICS_API::D3D11},
    {"DX11", GRAPHICS_API::D3D11},
    {"D3D11", GRAPHICS_API::D3D11},
    {"VULKAN", GRAPHICS_API::VK},
    {"VK", GRAPHICS_API::VK},
    {"DIRECTX 12", GRAPHICS_API::D3D12},
    {"DIRECT3D 12", GRAPHICS_API::D3D12},
    {"DX12", GRAPHICS_API::D3D12},
    {"D3D12", GRAPHICS_API::D3D12}
};

Satoshi::GRAPHICS_API Satoshi::RendererAPI::MatchAPIByName(std::string_view name)
{
    return s_APINamesMapper[name];
}
