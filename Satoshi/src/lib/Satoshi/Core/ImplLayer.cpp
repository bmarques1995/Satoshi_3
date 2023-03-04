#include "Satoshi/Core/ImplLayer.hpp"
#include "Satoshi.hpp"

Satoshi::ImplLayer::ImplLayer(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements)
{
    auto currentAPI = Application::GetInstance()->GetCurrentAPI();
    m_Shader.reset(Shader::Create(shaderGroup, elements, currentAPI));
    m_VertexBuffer.reset(VertexBuffer::Create(vBuffer, sizeof(vBuffer), m_Shader->GetLayout(), currentAPI));
    m_IndexBuffer.reset(IndexBuffer::Create(iBuffer, sizeof(iBuffer)/sizeof(uint32_t), currentAPI));
    
    m_VertexBuffer->Stage();
    m_IndexBuffer->Stage();
    m_VertexBuffer->RegisterLayout();

    //vkData = std::any_cast<VKData>(Application::GetInstance()->GetContext()->GetContextRunners());

    //CreateVertexBuffer(vBuffer, sizeof(vBuffer));
    //CreateIndexBuffer(iBuffer, sizeof(iBuffer)/sizeof(uint32_t));
}

void Satoshi::ImplLayer::OnAttach()
{
}

void Satoshi::ImplLayer::OnDetach()
{
}

void Satoshi::ImplLayer::OnUpdate()
{
	if (Input::IsKeyPressed(ST_KEY_C))
		Console::Log("Key C was pressed");
//    m_VertexBuffer->Stage();
//    m_IndexBuffer->Stage();
    
    m_Shader->Stage();
    m_VertexBuffer->Stage();
    m_IndexBuffer->Stage();
    //VkDeviceSize offset = 0;
    //vkCmdBindVertexBuffers(vkData.CommandBuffer, 0, 1, &m_VertexBuffer, &offset);
    //vkCmdBindIndexBuffer(vkData.CommandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void Satoshi::ImplLayer::OnEvent(Event& event)
{
}

/*
void Satoshi::ImplLayer::CreateVertexBuffer(const void* data, size_t size)
{
    VkDeviceSize bufferSize = size;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* rawData;
    vkMapMemory(vkData.Device, stagingBufferMemory, 0, bufferSize, 0, &rawData);
    memcpy(rawData, data, (size_t)bufferSize);
    vkUnmapMemory(vkData.Device, stagingBufferMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

    CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

    vkDestroyBuffer(vkData.Device, stagingBuffer, nullptr);
    vkFreeMemory(vkData.Device, stagingBufferMemory, nullptr);
}

void Satoshi::ImplLayer::CreateIndexBuffer(const void* data, size_t count)
{
    m_IndexCount = count;
    VkDeviceSize bufferSize = sizeof(uint32_t) * count;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* rawData;
    vkMapMemory(vkData.Device, stagingBufferMemory, 0, bufferSize, 0, &rawData);
    memcpy(rawData, data, (size_t)bufferSize);
    vkUnmapMemory(vkData.Device, stagingBufferMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

    CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

    vkDestroyBuffer(vkData.Device, stagingBuffer, nullptr);
    vkFreeMemory(vkData.Device, stagingBufferMemory, nullptr);
}

void Satoshi::ImplLayer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vkData.Device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vkData.Device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vkData.Device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(vkData.Device, buffer, bufferMemory, 0);
}

void Satoshi::ImplLayer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vkData.CommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(vkData.Device, &allocInfo, &commandBuffer);

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

    vkQueueSubmit(vkData.GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vkData.GraphicsQueue);

    vkFreeCommandBuffers(vkData.Device, vkData.CommandPool, 1, &commandBuffer);
}

uint32_t Satoshi::ImplLayer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vkData.PhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}
*/