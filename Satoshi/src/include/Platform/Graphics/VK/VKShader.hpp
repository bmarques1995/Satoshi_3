#ifndef VK_SHADER_HPP
#define VK_SHADER_HPP

#include "Satoshi/Renderer/Shader.hpp"
#include "Platform/Graphics/VK/VKContext.hpp"

namespace Satoshi
{
    class VKShader : public Shader
    {
    public:
        VKShader(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements);
        ~VKShader();

        virtual void Stage() const override;
        virtual const uint32_t GetOffset() const override;

        virtual const BufferLayout& GetLayout() const override;
    private:
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_GraphicsPipeline;
        VkCommandBuffer m_CommandBuffer;
        BufferLayout m_Layout;

        static std::unordered_map<GRAPHICS_SHADER_KIND, VkShaderStageFlagBits> s_VulkanShaderNames;

        VkFormat GetNativeFormat(ShaderDataType type);
        VkShaderModule CreateShaderModule(const void* bytecode, size_t size, VkDevice device);
        
    };
}


#endif //VK_SHADER_HPP
