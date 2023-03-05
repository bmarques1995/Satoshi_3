#ifndef GL4_SHADER_COMPILER_HPP
#define GL4_SHADER_COMPILER_HPP

#include "Satoshi/Renderer/ShaderCompiler.hpp"

namespace Satoshi
{
    class GL4ShaderCompiler : public ShaderCompiler
    {
    public:
        GL4ShaderCompiler();
        ~GL4ShaderCompiler();

        virtual COMPILE_ERRORS CompileFromFile(std::string_view baseShaderPath, std::string_view entrypoint, GRAPHICS_SHADER_KIND shaderKind, SHADER_VERSION shaderVersion, uint32_t optimizationLevel, uint32_t extraFlags = 0) override;
        virtual const std::unordered_map<GRAPHICS_SHADER_KIND, std::string_view> GetShaderKindStringMap() override;
        virtual const std::unordered_map<SHADER_VERSION, std::string_view> GetShaderVersion() override;
    
    private:
        
    };
}


#endif //D3D12_SHADER_MANAGER_HPP
