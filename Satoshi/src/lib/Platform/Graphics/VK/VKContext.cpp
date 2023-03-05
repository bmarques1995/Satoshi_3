#include "Platform/Graphics/VK/VKContext.hpp"
#include "Satoshi/Core/Application.hpp"
#include "Satoshi/Core/Console.hpp"

Satoshi::VKContext::VKContext(StWindowHandle window, uint32_t maxFramesInFlight) :
	m_WindowHandle(window), m_MaxFramesInFlight(maxFramesInFlight), m_VSync(false)
{

	//(195, 255, 104);

	m_ClearColor[0] = 195.0f / 255.0f;
	m_ClearColor[1] = 1.0f;
	m_ClearColor[2] = 104.0f / 255.0f;
	m_ClearColor[3] = 1.0f;

	CreateInstance();
	CreateSurface();
	CreatePhysicalDevice();
	CreateLogicalDevice();
	CreateQueue();
	CreateSwapChain();

	CreateImageViews();
	CreateRenderPass();
	CreateFramebuffers();

	CreateCommandPool();
	CreateCommandBuffer();
	CreateSyncObjects();
	CreateViewport();
	CreateAllocator();
}

Satoshi::VKContext::~VKContext()
{
	vkDestroySemaphore(m_LogicalDevice, m_FinishedSemaphore, nullptr);
	vkDestroySemaphore(m_LogicalDevice, m_AvailableSemaphore, nullptr);
	vkDestroyFence(m_LogicalDevice, m_Fence, nullptr);

	vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);;
	CleanupFramebuffers();
	vkDestroyRenderPass(m_LogicalDevice, m_RenderPass, nullptr);
	CleanupImageViews();
	CleanupSwapChain();
	vkDestroyDevice(m_LogicalDevice, nullptr);


#if defined(DEBUG) || defined(_DEBUG) 
	m_DebuggerDestroyer(m_Instance, m_Debugger, nullptr);

	m_DebuggerDestroyer = nullptr;
	m_DebuggerCreator = nullptr;
#endif

	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	vkDestroyInstance(m_Instance, nullptr);
}

void Satoshi::VKContext::ClearTarget()
{

}

void Satoshi::VKContext::SetClearColor(float r, float g, float b, float a)
{
	m_ClearColor[0] = r;
	m_ClearColor[1] = g;
	m_ClearColor[2] = b;
	m_ClearColor[3] = a;
}

void Satoshi::VKContext::ReceiveCommands()
{
	VkResult vk_r;
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkClearValue clearColor{};
	clearColor.color.float32[0] = m_ClearColor[0];
	clearColor.color.float32[1] = m_ClearColor[1];
	clearColor.color.float32[2] = m_ClearColor[2];
	clearColor.color.float32[3] = m_ClearColor[3];

	m_CurrentFrame.RenderPassInfo.clearValueCount = 1;
	m_CurrentFrame.RenderPassInfo.pClearValues = &clearColor;
	m_CurrentFrame.RenderPassInfo.pNext = nullptr;

	//m_CurrentFrame.renderPassInfo
	m_CurrentFrame.RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	m_CurrentFrame.RenderPassInfo.renderPass = m_RenderPass;
	m_CurrentFrame.RenderPassInfo.framebuffer = m_SwapChainFramebuffers[m_CurrentFrame.ImageIndex];
	m_CurrentFrame.RenderPassInfo.renderArea.offset = { 0, 0 };
	m_CurrentFrame.RenderPassInfo.renderArea.extent = m_SwapChainExtent;

	vkCmdBeginRenderPass(m_CommandBuffer, &m_CurrentFrame.RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Satoshi::VKContext::DispatchCommands()
{
	VkResult vk_r;

	vkCmdEndRenderPass(m_CommandBuffer);

	vk_r = vkEndCommandBuffer(m_CommandBuffer);
	assert(vk_r == VK_SUCCESS);
}

void Satoshi::VKContext::Draw(uint32_t elements)
{
	vkCmdSetScissor(m_CommandBuffer, 0, 1, &m_ScissorRect);
	vkCmdSetViewport(m_CommandBuffer, 0, 1, &m_Viewport);
	vkCmdSetPrimitiveTopology(m_CommandBuffer, VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	vkCmdDrawIndexed(m_CommandBuffer, elements, 1, 0, 0, 0);
}

void Satoshi::VKContext::NewFrame()
{
	VkResult result = vkAcquireNextImageKHR(m_LogicalDevice, m_SwapChain, UINT64_MAX, m_AvailableSemaphore, VK_NULL_HANDLE, &m_CurrentFrame.ImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapchain();
		result = vkAcquireNextImageKHR(m_LogicalDevice, m_SwapChain, UINT64_MAX, m_AvailableSemaphore, VK_NULL_HANDLE, &m_CurrentFrame.ImageIndex);
	}

	vkResetCommandBuffer(m_CommandBuffer, 0);
}

void Satoshi::VKContext::EndFrame()
{
	//vkGetFenceStatus();
	vkDeviceWaitIdle(m_LogicalDevice);
}

void Satoshi::VKContext::Present()
{
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_AvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffer;

	VkSemaphore signalSemaphores[] = { m_FinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkWaitForFences(m_LogicalDevice, 1, &m_Fence, VK_TRUE, UINT64_MAX);
	vkResetFences(m_LogicalDevice, 1, &m_Fence);
	VkResult vk_r = vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_Fence);
	assert(vk_r == VK_SUCCESS);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_SwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &m_CurrentFrame.ImageIndex;

	vkQueuePresentKHR(m_PresentQueue, &presentInfo);
}

bool Satoshi::VKContext::IsVSync()
{
	return m_VSync;
}

void Satoshi::VKContext::SetVSync(bool isVSync)
{
	m_VSync = isVSync;
	RecreateSwapchain();
}

void Satoshi::VKContext::GetGPUName(std::string* output)
{
	*output = m_DeviceProperties.deviceName;
}

void Satoshi::VKContext::OnResize(WindowResizeEvent& e)
{
	vkDeviceWaitIdle(m_LogicalDevice);

	RecreateSwapchain();
	CreateViewport();
}

std::any Satoshi::VKContext::GetContextRunners()
{
	VKData vkData = { m_LogicalDevice, m_CurrentDevice, m_CommandBuffer, m_CommandPool, m_RenderPass, m_GraphicsQueue, m_Allocator };
	return vkData;
}

void Satoshi::VKContext::CreateInstance()
{
	VkResult vk_r;

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 3, 236);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 3, 236);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInstanceInfo{};
	createInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInstanceInfo.pApplicationInfo = &appInfo;

	uint32_t extensionCount = 0;
	vk_r = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	assert(vk_r == VK_SUCCESS);

	std::vector<VkExtensionProperties> extensionProperties(extensionCount);
	vk_r = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());
	assert(vk_r == VK_SUCCESS);

	std::vector<const char*> extensions;
	for (auto& extension : extensionProperties) {
		extensions.push_back(extension.extensionName);
	}

#if defined(DEBUG) || defined(_DEBUG) 
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	createInstanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInstanceInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
#if defined(DEBUG) || defined(_DEBUG) 
	createInstanceInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
	createInstanceInfo.ppEnabledLayerNames = m_ValidationLayers.data();

	debugCreateInfo = {};
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugCreateInfo.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32
	{
#ifdef _WIN32
		OutputDebugStringA("validation layer: ");
		OutputDebugStringA(pCallbackData->pMessage);
		OutputDebugStringA("\n");
#else
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
#endif // _WIN32

		return VK_FALSE;
	};
	createInstanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#else
	createInstanceInfo.enabledLayerCount = 0;

	createInstanceInfo.pNext = nullptr;
#endif


	vk_r = vkCreateInstance(&createInstanceInfo, nullptr, &m_Instance);
	assert(vk_r == VK_SUCCESS);

#if defined(DEBUG) || defined(_DEBUG) 
	m_DebuggerCreator = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
	assert(m_DebuggerCreator != nullptr);

	m_DebuggerDestroyer = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
	assert(m_DebuggerDestroyer != nullptr);

	vk_r = m_DebuggerCreator(m_Instance, &debugCreateInfo, nullptr, &m_Debugger);
	assert(vk_r == VK_SUCCESS);
#endif
}

void Satoshi::VKContext::CreateSurface()
{
	VkResult vk_r;

#if 0 //native wayland surface
	VkWaylandSurfaceCreateInfoKHR waylandInfo{};
	waylandInfo.display = glfwGetWaylandDisplay();
	waylandInfo.surface = glfwGetWaylandWindow(window);
	waylandInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;

	vk_r = vkCreateWin32SurfaceKHR(m_Instance, &waylandInfo, nullptr, &m_Surface);
	assert(vk_r == VK_SUCCESS);
#endif

#if ST_PLATFORM_WINDOWS //native win32 surface
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = m_WindowHandle;
	createInfo.hinstance = GetModuleHandle(nullptr);

	vk_r = vkCreateWin32SurfaceKHR(m_Instance, &createInfo, nullptr, &m_Surface);
	assert(vk_r == VK_SUCCESS);
#endif // _WIN32

}

void Satoshi::VKContext::CreatePhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

	for (auto& device : devices)
	{
		uint32_t score = RatePhysicalDevice(device);
		while (m_Devices.find(score) != m_Devices.end())
			score++;
		if (score != 0)
			m_Devices.insert(std::make_pair(score, device));
	}

	SelectPhysicalDevice();
	bool extensionsSupported = CheckDeviceExtensionSupport(m_CurrentDevice);

	assert((deviceCount != 0) && extensionsSupported);
}

void Satoshi::VKContext::CreateLogicalDevice()
{
	VkResult vk_r;

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::vector<uint32_t> uniqueQueueFamilies;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_CurrentDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_CurrentDevice, &queueFamilyCount, queueFamilies.data());

	uint32_t i = 0;
	for (auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			m_GraphicsQueueFamilyIndex = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_CurrentDevice, i, m_Surface, &presentSupport);

		if (presentSupport) {
			m_PresentQueueFamilyIndex = i;
		}
		i++;
	}

	uniqueQueueFamilies.push_back(m_GraphicsQueueFamilyIndex);
	uniqueQueueFamilies.push_back(m_PresentQueueFamilyIndex);

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = (uint32_t)m_DeviceExtensions.size();
	createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

#if defined(DEBUG) || defined(_DEBUG)
	createInfo.enabledLayerCount = (uint32_t)m_ValidationLayers.size();
	createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
#else
	createInfo.enabledLayerCount = 0;
#endif

	vk_r = vkCreateDevice(m_CurrentDevice, &createInfo, nullptr, &m_LogicalDevice);
	assert(vk_r == VK_SUCCESS);
}

void Satoshi::VKContext::CreateQueue()
{
	vkGetDeviceQueue(m_LogicalDevice, m_GraphicsQueueFamilyIndex, 0, &m_GraphicsQueue);
	vkGetDeviceQueue(m_LogicalDevice, m_PresentQueueFamilyIndex, 0, &m_PresentQueue);
}

void Satoshi::VKContext::CreateSwapChain()
{
	VkResult vk_r;

	VKSwapChainSupportDetails details = QuerySwapChainSupport(m_CurrentDevice);
#if defined(DEBUG) || defined(_DEBUG) 
	m_SwapChainDetails = details;
#endif
	//const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
	//const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(details.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(details.presentModes);
	VkExtent2D extent = ChooseSwapExtent(details.capabilities, m_WindowHandle);
	m_MinImageCount = details.capabilities.minImageCount;
	m_ImageCount = m_MinImageCount + 1;
	if (details.capabilities.maxImageCount > 0 && m_ImageCount > details.capabilities.maxImageCount) {
		m_ImageCount = details.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Surface;

	createInfo.minImageCount = m_ImageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { m_GraphicsQueueFamilyIndex, m_PresentQueueFamilyIndex };

	if (m_GraphicsQueueFamilyIndex != m_PresentQueueFamilyIndex) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = details.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	vk_r = vkCreateSwapchainKHR(m_LogicalDevice, &createInfo, nullptr, &m_SwapChain);
	assert(vk_r == VK_SUCCESS);

	vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &m_ImageCount, nullptr);
	m_SwapChainImages.resize(m_ImageCount);
	vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &m_ImageCount, m_SwapChainImages.data());

	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent = extent;
}

void Satoshi::VKContext::CreateImageViews()
{
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	VkResult vk_r;

	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_SwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_SwapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		vk_r = vkCreateImageView(m_LogicalDevice, &createInfo, nullptr, &m_SwapChainImageViews[i]);
		assert(vk_r == VK_SUCCESS);
	}
}

void Satoshi::VKContext::CreateRenderPass()
{
	VkResult vk_r;
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_SwapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo RenderPassInfo{};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassInfo.attachmentCount = 1;
	RenderPassInfo.pAttachments = &colorAttachment;
	RenderPassInfo.subpassCount = 1;
	RenderPassInfo.pSubpasses = &subpass;

	vk_r = vkCreateRenderPass(m_LogicalDevice, &RenderPassInfo, nullptr, &m_RenderPass);
	assert(vk_r == VK_SUCCESS);
}

void Satoshi::VKContext::CreateFramebuffers()
{
	VkResult vk_r;
	m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

	for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
			m_SwapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_SwapChainExtent.width;
		framebufferInfo.height = m_SwapChainExtent.height;
		framebufferInfo.layers = 1;

		vk_r = vkCreateFramebuffer(m_LogicalDevice, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]);
		assert(vk_r == VK_SUCCESS);
	}
}

void Satoshi::VKContext::CreateCommandPool()
{
	VkResult vk_r;
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;

	vk_r = vkCreateCommandPool(m_LogicalDevice, &poolInfo, nullptr, &m_CommandPool);
	assert(vk_r == VK_SUCCESS);
}

void Satoshi::VKContext::CreateCommandBuffer()
{
	VkResult vk_r;
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	vk_r = vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, &m_CommandBuffer);
	assert(vk_r == VK_SUCCESS);
}

void Satoshi::VKContext::CreateSyncObjects()
{
	VkResult vk_r;
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	vk_r = vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, &m_CommandBuffer);
	assert(vk_r == VK_SUCCESS);

	vk_r = vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_AvailableSemaphore);
	assert(vk_r == VK_SUCCESS);
	vk_r = vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_FinishedSemaphore);
	assert(vk_r == VK_SUCCESS);
	vk_r = vkCreateFence(m_LogicalDevice, &fenceInfo, nullptr, &m_Fence);
	assert(vk_r == VK_SUCCESS);
}

void Satoshi::VKContext::CleanupSwapChain()
{
	vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, nullptr);
}

void Satoshi::VKContext::CleanupImageViews()
{
	for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
		vkDestroyImageView(m_LogicalDevice, m_SwapChainImageViews[i], nullptr);
	}
}

void Satoshi::VKContext::CleanupFramebuffers()
{
	for (size_t i = 0; i < m_SwapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(m_LogicalDevice, m_SwapChainFramebuffers[i], nullptr);
	}
}

void Satoshi::VKContext::RecreateSwapchain()
{
	vkDeviceWaitIdle(m_LogicalDevice);

	CleanupFramebuffers();
	CleanupImageViews();
	CleanupSwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateFramebuffers();
}

void Satoshi::VKContext::CreateViewport()
{
	m_Viewport.x = 0.0f;
	m_Viewport.y = 0.0f;
	m_Viewport.width = (float)m_SwapChainExtent.width;
	m_Viewport.height = (float)m_SwapChainExtent.height;
	m_Viewport.minDepth = 0.0f;
	m_Viewport.maxDepth = 1.0f;

	m_ScissorRect.offset = { 0, 0 };
	m_ScissorRect.extent = m_SwapChainExtent;
}

void Satoshi::VKContext::CreateAllocator()
{
	VkResult vk_r;
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = m_CurrentDevice;
	allocatorInfo.device = m_LogicalDevice;
	allocatorInfo.instance = m_Instance;

	vk_r = vmaCreateAllocator(&allocatorInfo, &m_Allocator);
	assert(vk_r == VK_SUCCESS);
}

uint32_t Satoshi::VKContext::RatePhysicalDevice(VkPhysicalDevice device)
{
	uint32_t score = 0;

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &m_DeviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	if (m_DeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	// Maximum possible size of textures affects graphics quality
	score += m_DeviceProperties.limits.maxImageDimension2D;

	// Application can't function without geometry shaders
	if (!deviceFeatures.geometryShader || !IsDeviceSuitable(device)) {
		score = 0;
	}
	return score;
}

void Satoshi::VKContext::SelectPhysicalDevice()
{
	m_CurrentDevice = m_Devices.begin()->second;
	vkGetPhysicalDeviceProperties(m_CurrentDevice, &m_DeviceProperties);
}

bool Satoshi::VKContext::IsDeviceSuitable(VkPhysicalDevice device)
{
	std::optional<uint32_t> graphicsQueue;
	std::optional<uint32_t> presentQueue;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	uint32_t i = 0;
	for (auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsQueue = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

		if (presentSupport) {
			presentQueue = i;
		}
		i++;
	}

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		VKSwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return (graphicsQueue.has_value()) && (presentQueue.has_value()) && extensionsSupported && swapChainAdequate;

}

bool Satoshi::VKContext::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

Satoshi::VKSwapChainSupportDetails Satoshi::VKContext::QuerySwapChainSupport(VkPhysicalDevice device)
{
	VKSwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR Satoshi::VKContext::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR Satoshi::VKContext::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	if (m_VSync)
	{
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}
	else
	{
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
	}
}

VkExtent2D Satoshi::VKContext::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, HWND window)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		RECT area;
		GetClientRect(window, &area);

		width = (int)(area.right - area.left);
		height = (int)(area.bottom - area.top);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}
