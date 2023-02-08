#ifdef ST_PLATFORM_WINDOWS

#ifndef D3D11_SHADER_MANAGER_HPP
#define D3D11_SHADER_MANAGER_HPP

#include "Satoshi/Renderer/ShaderManager.hpp"
#include "Platform/Graphics/D3D11/D3D11ShaderCompiler.hpp"

namespace Satoshi
{
    class D3D11ShaderManager : public ShaderManager
    {
    public:
        D3D11ShaderManager();
        ~D3D11ShaderManager();

        virtual void CompileShaderFamily(const ShaderGroup& shaderGroup) override;
        virtual const std::string& BuildBlobFilename(std::string_view baseShaderPath, SHADER_KIND shaderKind, SHADER_VERSION shaderVersion) override;

    private:
        ShaderCompiler* m_Compiler;
    };
}


#endif //D3D12_SHADER_MANAGER_HPP

#endif 
