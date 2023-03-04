#include "Satoshi/Core/ImplLayer.hpp"

#include "Satoshi.hpp"

std::unordered_map<Satoshi::SHADER_KIND, VkShaderStageFlagBits> Satoshi::ImplLayer::s_VulkanShaderNames = 
{
    {SHADER_KIND::SHADER_KIND_VERTEX, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT},
    {SHADER_KIND::SHADER_KIND_PIXEL, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT},
    {SHADER_KIND::SHADER_KIND_GEOMETRY, VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT},
    {SHADER_KIND::SHADER_KIND_HULL, VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT},
    {SHADER_KIND::SHADER_KIND_DOMAIN, VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
    {SHADER_KIND::SHADER_KIND_COMPUTE, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT}
};

Satoshi::ImplLayer::ImplLayer(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements) :
    m_Layout(elements)
{
    auto currentAPI = Application::GetInstance()->GetCurrentAPI();
    //m_Shader.reset(Shader::Create(shaderGroup, elements, currentAPI));
    //m_VertexBuffer.reset(VertexBuffer::Create(vBuffer, sizeof(vBuffer), m_Shader->GetLayout(), currentAPI));
    //m_IndexBuffer.reset(IndexBuffer::Create(iBuffer, sizeof(iBuffer)/sizeof(uint32_t), currentAPI));
    
    //m_VertexBuffer->Stage();
    //m_IndexBuffer->Stage();
    //m_VertexBuffer->RegisterLayout();

    vkData = std::any_cast<VKData>(Application::GetInstance()->GetContext()->GetContextRunners());
    CreateShader(shaderGroup, elements);
    CreateVertexBuffer(vBuffer, sizeof(vBuffer));
    CreateIndexBuffer(iBuffer, sizeof(iBuffer)/sizeof(uint32_t));
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
    vkCmdBindPipeline(vkData.CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(vkData.CommandBuffer, 0, 1, &m_VertexBuffer, &offset);
    vkCmdBindIndexBuffer(vkData.CommandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void Satoshi::ImplLayer::OnEvent(Event& event)
{
}

void Satoshi::ImplLayer::CreateShader(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements)
{
    VkResult vk_r;
    auto details = shaderGroup.GetShaderDetails();
    auto shaderManager = Application::GetInstance()->GetShaderManager();
    VkPipelineShaderStageCreateInfo* shaderStages = new VkPipelineShaderStageCreateInfo[details.size()];
    VkShaderModule* shaderModules = new VkShaderModule[details.size()];

    for (size_t i = 0; i < details.size(); i++)
    {
        std::byte* bytecode;
        size_t size;
        FileHandler::ReadBinFile(shaderManager->BuildBlobFilename(shaderGroup.GetBasePath(), details[i].ShaderKind, shaderGroup.GetShaderVersion()),&bytecode,&size);

        shaderModules[i] = CreateShaderModule(bytecode, size);
        VkPipelineShaderStageCreateInfo shaderStageInfo{};
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.stage = s_VulkanShaderNames[details[i].ShaderKind];
        shaderStageInfo.module = shaderModules[i];
        shaderStageInfo.pName = "main";
        shaderStages[i] = shaderStageInfo;
    }

    auto nativeElements = m_Layout.GetElements();

#pragma region InputLayout
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = m_Layout.GetStride();
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


    VkVertexInputAttributeDescription* attributeDescriptions = new VkVertexInputAttributeDescription[nativeElements.size()];

    for (size_t i = 0; i < nativeElements.size(); i++)
    {
        attributeDescriptions[i].binding = 0;
        attributeDescriptions[i].location = i;
        attributeDescriptions[i].format = GetNativeFormat(nativeElements[i].Type);
        attributeDescriptions[i].offset = nativeElements[i].Offset;
    }

#pragma endregion

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = nativeElements.size();
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    vk_r = vkCreatePipelineLayout(vkData.Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout);
    assert(vk_r == VK_SUCCESS);

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = details.size();
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_PipelineLayout;
    pipelineInfo.renderPass = vkData.RenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    vk_r = vkCreateGraphicsPipelines(vkData.Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline);
    assert(vk_r == VK_SUCCESS);

    for (size_t i = 0; i < details.size(); i++)
    {
        vkDestroyShaderModule(vkData.Device, shaderStages[i].module, nullptr);
    }

    delete[] shaderStages;
    delete[] shaderModules;
    delete[] attributeDescriptions;
}

VkShaderModule Satoshi::ImplLayer::CreateShaderModule(const void* bytecode, size_t size) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = size;
    createInfo.pCode = reinterpret_cast<const uint32_t*>(bytecode);
    createInfo.pNext = nullptr;

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(vkData.Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

VkFormat Satoshi::ImplLayer::GetNativeFormat(ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float: return VK_FORMAT_R32_SFLOAT;
    case ShaderDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
    case ShaderDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
    case ShaderDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
    case ShaderDataType::Uint: return VK_FORMAT_R32_UINT;
    case ShaderDataType::Uint2: return VK_FORMAT_R32G32_UINT;
    case ShaderDataType::Uint3: return VK_FORMAT_R32G32B32_UINT;
    case ShaderDataType::Uint4: return VK_FORMAT_R32G32B32A32_UINT;
    case ShaderDataType::Bool: return VK_FORMAT_R8_UINT;
    default: return VK_FORMAT_UNDEFINED;
    }
}

void Satoshi::ImplLayer::CreateVertexBuffer(const void* data, size_t size)
{
    VkDeviceSize bufferSize = size;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* rawData;
    vkMapMemory(vkData.Device, stagingBufferMemory, 0, bufferSize, 0, &rawData);
    memcpy(rawData, data, (size_t)bufferSize);
    vkUnmapMemory(vkData.Device, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

    copyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

    vkDestroyBuffer(vkData.Device, stagingBuffer, nullptr);
    vkFreeMemory(vkData.Device, stagingBufferMemory, nullptr);
}

void Satoshi::ImplLayer::CreateIndexBuffer(const void* data, size_t count)
{
    m_IndexCount = count;
    VkDeviceSize bufferSize = sizeof(uint32_t) * count;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* rawData;
    vkMapMemory(vkData.Device, stagingBufferMemory, 0, bufferSize, 0, &rawData);
    memcpy(rawData, data, (size_t)bufferSize);
    vkUnmapMemory(vkData.Device, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

    copyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

    vkDestroyBuffer(vkData.Device, stagingBuffer, nullptr);
    vkFreeMemory(vkData.Device, stagingBufferMemory, nullptr);
}

void Satoshi::ImplLayer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
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
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vkData.Device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(vkData.Device, buffer, bufferMemory, 0);
}

void Satoshi::ImplLayer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
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

uint32_t Satoshi::ImplLayer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
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
