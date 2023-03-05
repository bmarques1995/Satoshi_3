#include "Platform/Graphics/VK/VKBuffer.hpp"
#include "Satoshi.hpp"

Satoshi::VKVertexBuffer::VKVertexBuffer(const void* data, size_t size, uint32_t stride) :
    m_Stride(stride)
{
    VkResult vk_r;
    VKData vkData = std::any_cast<VKData>(Application::GetInstance()->GetContext()->GetContextRunners());
    m_CommandBuffer = vkData.CommandBuffer;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    VmaAllocation m_Allocation;
    VmaAllocationInfo m_AllocationInfo;
    vk_r = vmaCreateBuffer(vkData.Allocator, &bufferCreateInfo, &allocCreateInfo, &m_VertexBuffer, &m_Allocation, &m_AllocationInfo);
    assert(vk_r == VK_SUCCESS);

    // Copy vertex data to buffer
    void* rawData;
    vmaMapMemory(vkData.Allocator, m_Allocation, &rawData);
    memcpy(rawData, data, bufferCreateInfo.size);
    vmaUnmapMemory(vkData.Allocator, m_Allocation);
}

Satoshi::VKVertexBuffer::~VKVertexBuffer()
{
    VKData vkData = std::any_cast<VKData>(Application::GetInstance()->GetContext()->GetContextRunners());
    vkDestroyBuffer(vkData.Device, m_VertexBuffer, nullptr);
}

void Satoshi::VKVertexBuffer::Stage() const
{
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &m_VertexBuffer, &offset);
}

void Satoshi::VKVertexBuffer::RegisterLayout(IndexBuffer* indexBuffer)
{
}

Satoshi::VKIndexBuffer::VKIndexBuffer(const void* data, size_t count)
{
    m_Count = count;
    VkResult vk_r;
    VkDeviceSize bufferSize = sizeof(uint32_t) * count;
    VKData vkData = std::any_cast<VKData>(Application::GetInstance()->GetContext()->GetContextRunners());
    m_CommandBuffer = vkData.CommandBuffer;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = count * sizeof(uint32_t);
    bufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    VmaAllocation m_Allocation;
    VmaAllocationInfo m_AllocationInfo;
    vk_r = vmaCreateBuffer(vkData.Allocator, &bufferCreateInfo, &allocCreateInfo, &m_IndexBuffer, &m_Allocation, &m_AllocationInfo);
    assert(vk_r == VK_SUCCESS);

    // Copy vertex data to buffer
    void* rawData;
    vmaMapMemory(vkData.Allocator, m_Allocation, &rawData);
    memcpy(rawData, data, bufferCreateInfo.size);
    vmaUnmapMemory(vkData.Allocator, m_Allocation);
}

Satoshi::VKIndexBuffer::~VKIndexBuffer()
{
    VKData vkData = std::any_cast<VKData>(Application::GetInstance()->GetContext()->GetContextRunners());
    vkDestroyBuffer(vkData.Device, m_IndexBuffer, nullptr);
}

void Satoshi::VKIndexBuffer::Stage() const
{
    vkCmdBindIndexBuffer(m_CommandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

uint32_t Satoshi::VKIndexBuffer::GetCount() const
{
    return 0;
}
