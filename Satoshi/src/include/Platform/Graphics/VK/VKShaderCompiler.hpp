#ifndef VK_SHADER_COMPILER_HPP
#define VK_SHADER_COMPILER_HPP

#include "Satoshi/Renderer/ShaderCompiler.hpp"
#include <shaderc/shaderc.hpp>

namespace Satoshi
{
    class VKShaderCompiler : public ShaderCompiler
    {
    public:
        VKShaderCompiler(bool useHLSL);
        ~VKShaderCompiler();

        virtual COMPILE_ERRORS CompileFromFile(std::string_view baseShaderPath, std::string_view entrypoint, GRAPHICS_SHADER_KIND shaderKind, SHADER_VERSION shaderVersion, uint32_t optimizationLevel, uint32_t extraFlags = 0) override;
        virtual const std::unordered_map<GRAPHICS_SHADER_KIND, std::string_view> GetShaderKindStringMap() override;
        virtual const std::unordered_map<SHADER_VERSION, std::string_view> GetShaderVersion() override;
    private:

        std::string s_Target;
        std::string s_InputFile;
        std::string s_OutputFile;

        bool m_UseHLSL;

        void BuildInputFile(GRAPHICS_SHADER_KIND shaderKind, std::string_view baseShaderPath);
        void BuildOutputFile(GRAPHICS_SHADER_KIND shaderKind, SHADER_VERSION shaderVersion, std::string_view baseShaderPath);

        static std::unordered_map<GRAPHICS_SHADER_KIND, shaderc_shader_kind> s_MappedShaderKinds;
    };
}


#endif //D3D12_SHADER_MANAGER_HPP
