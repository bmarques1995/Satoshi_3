#ifndef GL4_SHADER_HPP
#define GL4_SHADER_HPP

#include "Satoshi/Renderer/Shader.hpp"
#include "Platform/Graphics/GL4/GL4Context.hpp"

namespace Satoshi
{
    class GL4Shader : public Shader
    {
    public:
        GL4Shader(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements);
        ~GL4Shader();

        virtual void Stage() const override;
        virtual const uint32_t GetOffset() const override;

        virtual const BufferLayout& GetLayout() const override;
    private:
        uint32_t CompileShader(std::string_view filepath, GLenum shaderKind);
        bool GotCompileErrors(uint32_t shaderID);
        uint32_t LinkShaders(std::list<uint32_t> shaders);
        bool GotLinkErrors(uint32_t programID);

        ShaderGroup m_ShaderGroup;
        BufferLayout m_Layout;

        static std::unordered_map<SHADER_KIND, GLenum> s_NativeShaderEnums;

        uint32_t m_Shader;
    };
}


#endif //GL4_SHADER_HPP
