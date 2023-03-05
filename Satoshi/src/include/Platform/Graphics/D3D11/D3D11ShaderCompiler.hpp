#ifdef ST_PLATFORM_WINDOWS

#ifndef D3D11_SHADER_COMPILER_HPP
#define D3D11_SHADER_COMPILER_HPP

#include "Satoshi/Renderer/ShaderCompiler.hpp"
#include <d3dcompiler.h>

namespace Satoshi
{
    class D3D11ShaderCompiler : public ShaderCompiler
    {
    public:
        D3D11ShaderCompiler();
        ~D3D11ShaderCompiler();
    
        virtual COMPILE_ERRORS CompileFromFile(std::string_view baseShaderPath, std::string_view entrypoint, GRAPHICS_SHADER_KIND shaderKind, SHADER_VERSION shaderVersion, uint32_t optimizationLevel, uint32_t extraFlags = 0) override;
        virtual const std::unordered_map<GRAPHICS_SHADER_KIND, std::string_view> GetShaderKindStringMap() override;
        virtual const std::unordered_map<SHADER_VERSION, std::string_view> GetShaderVersion() override;
    private:
    
        std::string s_Target;
        std::string s_InputFile;
        std::string s_OutputFile;

        void BuildTarget(GRAPHICS_SHADER_KIND shaderKind, SHADER_VERSION hlslVersion);
        void BuildInputFile(GRAPHICS_SHADER_KIND shaderKind, std::string_view baseShaderPath);
        void BuildOutputFile(GRAPHICS_SHADER_KIND shaderKind, SHADER_VERSION hlslVersion, std::string_view baseShaderPath);
    };
    
}

#endif //D3D11_SHADER_MANAGER_HPP

#endif
