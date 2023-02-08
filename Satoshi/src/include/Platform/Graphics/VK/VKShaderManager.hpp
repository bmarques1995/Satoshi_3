#ifndef VK_SHADER_MANAGER_HPP
#define VK_SHADER_MANAGER_HPP

#include "Satoshi/Renderer/ShaderManager.hpp"
#include "Platform/Graphics/VK/VKShaderCompiler.hpp"

namespace Satoshi
{
    class VKShaderManager : public ShaderManager
    {
    public:
        VKShaderManager();
        ~VKShaderManager();

        virtual void CompileShaderFamily(const ShaderGroup& shaderGroup) override;
        virtual const std::string& BuildBlobFilename(std::string_view baseShaderPath, SHADER_KIND shaderKind, SHADER_VERSION shaderVersion) override;

    private:
        ShaderCompiler* m_Compiler;

    };
}


#endif //D3D12_SHADER_MANAGER_HPP
