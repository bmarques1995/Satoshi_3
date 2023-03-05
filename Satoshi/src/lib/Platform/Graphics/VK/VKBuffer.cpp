#include "Platform/Graphics/VK/VKBuffer.hpp"
#include "Satoshi.hpp"

void Satoshi::VKBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkDevice device, VkPhysicalDevice physicalDevice)
{
    VkResult vk_r;
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vk_r = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
    assert(vk_r == VK_SUCCESS);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice);

    vk_r = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
    assert(vk_r == VK_SUCCESS);

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void Satoshi::VKBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool, VkDevice device, VkQueue graphicsQueue)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

uint32_t Satoshi::VKBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

Satoshi::VKVertexBuffer::VKVertexBuffer(const void* data, size_t size, uint32_t stride) :
    m_Stride(stride)
{
    VKData vkData = std::any_cast<VKData>(Application::GetInstance()->GetContext()->GetContextRunners());
    m_CommandBuffer = vkData.CommandBuffer;
    VkDeviceSize bufferSize = size;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, vkData.Device, vkData.PhysicalDevice);

    void* rawData;
    vkMapMemory(vkData.Device, stagingBufferMemory, 0, bufferSize, 0, &rawData);
    memcpy(rawData, data, (size_t)bufferSize);
    vkUnmapMemory(vkData.Device, stagingBufferMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_BufferMemory, vkData.Device, vkData.PhysicalDevice);

    CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize, vkData.CommandPool, vkData.Device, vkData.GraphicsQueue);

    vkDestroyBuffer(vkData.Device, stagingBuffer, nullptr);
    vkFreeMemory(vkData.Device, stagingBufferMemory, nullptr);
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
    VkDeviceSize bufferSize = sizeof(uint32_t) * count;
    VKData vkData = std::any_cast<VKData>(Application::GetInstance()->GetContext()->GetContextRunners());
    m_CommandBuffer = vkData.CommandBuffer;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, vkData.Device, vkData.PhysicalDevice);

    void* rawData;
    vkMapMemory(vkData.Device, stagingBufferMemory, 0, bufferSize, 0, &rawData);
    memcpy(rawData, data, (size_t)bufferSize);
    vkUnmapMemory(vkData.Device, stagingBufferMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_BufferMemory, vkData.Device, vkData.PhysicalDevice);

    CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize, vkData.CommandPool, vkData.Device, vkData.GraphicsQueue);

    vkDestroyBuffer(vkData.Device, stagingBuffer, nullptr);
    vkFreeMemory(vkData.Device, stagingBufferMemory, nullptr);
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
