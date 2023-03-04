#ifndef IMPL_LAYER_HPP
#define IMPL_LAYER_HPP

#include <stpch.hpp>
#include "Layer.hpp"
#include "Satoshi/Renderer/Shader.hpp"
#include "Satoshi/Renderer/Buffer.hpp"

#include "Platform/Graphics/VK/VKContext.hpp"

namespace Satoshi
{
	

	class ImplLayer : public Layer
	{
	public:
		ImplLayer(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements);
		~ImplLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Event& event) override;
	private:
		
		void CreateShader(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements);
		VkShaderModule CreateShaderModule(const void* bytecode, size_t size);

		static std::unordered_map<SHADER_KIND, VkShaderStageFlagBits> s_VulkanShaderNames;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;

		BufferLayout m_Layout;

		VkFormat GetNativeFormat(ShaderDataType type);
#pragma region Shader

		std::shared_ptr<Shader> m_Shader;

		VKData vkData;

#pragma endregion

#pragma region Buffer

		float vBuffer[21] =
		{
			.0f, .5f, .0f, 1.0f, .0f, .0f, 1.0f,
			.5f, -.5f, .0f, .0f, 1.0f, .0f, 1.0f,
			-.5f, -.5f, .0f, .0f, .0f, 1.0f, 1.0f
		};

		uint32_t iBuffer[3] =
		{
			0,1,2
		};

		//std::shared_ptr<VertexBuffer> m_VertexBuffer;
		//std::shared_ptr<IndexBuffer> m_IndexBuffer;

		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMemory;
		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_IndexBufferMemory;
		uint32_t m_IndexCount;

		void CreateVertexBuffer(const void* data, size_t size);
		void CreateIndexBuffer(const void* data, size_t count);

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

#pragma endregion
	};
}

#endif //IMPL_LAYER_HPP
