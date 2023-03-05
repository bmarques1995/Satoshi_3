#ifndef VK_BUFFER_HPP
#define VK_BUFFER_HPP

#include "Satoshi/Renderer/Buffer.hpp"
#include "VKContext.hpp"

namespace Satoshi
{
    class VKBuffer
    {
	protected:
		VmaAllocation m_Allocation;
		VmaAllocationInfo m_AllocationInfo;
	};

	class VKVertexBuffer : public VertexBuffer, public VKBuffer
	{
	public:
		VKVertexBuffer(const void* data, size_t size, uint32_t stride);
		~VKVertexBuffer();

		virtual void Stage() const override;
		virtual void RegisterLayout(IndexBuffer* indexBuffer) override;
	private:
		uint32_t m_Stride;
		VkBuffer m_VertexBuffer;
		VkCommandBuffer_T* m_CommandBuffer;
	};

	class VKIndexBuffer : public IndexBuffer, public VKBuffer
	{
	public:
		VKIndexBuffer(const void* data, size_t count);
		~VKIndexBuffer();

		virtual void Stage() const override;
		virtual uint32_t GetCount() const override;
	private:
		VkBuffer m_IndexBuffer;
		VkCommandBuffer_T* m_CommandBuffer;
	};
}


#endif //VK_BUFFER_HPP
