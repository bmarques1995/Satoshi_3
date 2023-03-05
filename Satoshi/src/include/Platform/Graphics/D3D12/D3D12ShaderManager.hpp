#ifdef ST_PLATFORM_WINDOWS

#ifndef D3D12_SHADER_MANAGER_HPP
#define D3D12_SHADER_MANAGER_HPP

#include "Satoshi/Renderer/ShaderManager.hpp"
#include "Platform/Graphics/D3D12/D3D12ShaderCompiler.hpp"

namespace Satoshi
{
    class D3D12ShaderManager : public ShaderManager
    {
    public:
        D3D12ShaderManager();
        ~D3D12ShaderManager();

        virtual void CompileShaderFamily(const ShaderGroup& shaderGroup) override;
        virtual const std::string& BuildBlobFilename(std::string_view baseShaderPath, GRAPHICS_SHADER_KIND shaderKind, SHADER_VERSION shaderVersion) override;

    private:
        ShaderCompiler* m_Compiler;
    };
}


#endif //D3D12_SHADER_MANAGER_HPP

#endif 
