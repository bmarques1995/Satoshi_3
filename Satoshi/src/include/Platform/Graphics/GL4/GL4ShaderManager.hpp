#ifndef GL4_SHADER_MANAGER_HPP
#define GL4_SHADER_MANAGER_HPP

#include "Satoshi/Renderer/ShaderManager.hpp"
#include "Platform/Graphics/GL4/GL4ShaderCompiler.hpp"

namespace Satoshi
{
    class GL4ShaderManager : public ShaderManager
    {
    public:
        GL4ShaderManager();
        ~GL4ShaderManager();

        virtual void CompileShaderFamily(const ShaderGroup& shaderGroup) override;
        virtual const std::string& BuildBlobFilename(std::string_view baseShaderPath, SHADER_KIND shaderKind, SHADER_VERSION shaderVersion) override;
    private:
        ShaderCompiler* m_Compiler;
    };
}


#endif //D3D12_SHADER_MANAGER_HPP
