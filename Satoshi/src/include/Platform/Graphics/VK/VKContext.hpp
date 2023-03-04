#ifndef VK_CONTEXT_HPP
#define VK_CONTEXT_HPP

#include "Satoshi/Renderer/GraphicsContext.hpp"
#include <vulkan/vulkan.hpp>
#include <stpch.hpp>
#include "Satoshi/Core/Core.hpp"
#include "VKShaderManager.hpp"

#ifdef ST_PLATFORM_WINDOWS

#include <windows.h>
#include <vulkan/vulkan_win32.h>

#endif // ST_PLATFORM_WINDOWS

namespace Satoshi
{
    struct VKSwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct VKFrame
    {
        uint32_t ImageIndex; 
        VkRenderPassBeginInfo RenderPassInfo;
    };

    struct VKData
    {
        VkDevice_T* Device;
        VkPhysicalDevice_T* PhysicalDevice;
        VkCommandBuffer_T* CommandBuffer;
        VkCommandPool_T* CommandPool;
        VkRenderPass_T* RenderPass;
        VkQueue_T* GraphicsQueue;
        
    };

    class VKContext : public GraphicsContext
    {
    public:
        VKContext(StWindowHandle window, uint32_t maxFramesInFlight = 2);
        ~VKContext();

        virtual void ClearTarget() override;
        virtual void SetClearColor(float r, float g, float b, float a) override;
        virtual void ReceiveCommands() override;
        virtual void DispatchCommands() override;
        virtual void Draw(uint32_t elements) override;
        virtual void SetTopology() override;

        virtual void NewFrame() override;
        virtual void EndFrame() override;
        virtual void Present() override;

        virtual bool IsVSync() override;
        virtual void SetVSync(bool isVSync) override;

        virtual void GetGPUName(std::string* output) override;

        virtual void OnResize(WindowResizeEvent& e) override;

        virtual std::any GetContextRunners() override;

    private:
        

        void CreateInstance();
        void CreateSurface();
        void CreatePhysicalDevice();
        void CreateLogicalDevice();
        void CreateQueue();
        void CreateSwapChain();

        void CreateImageViews();
        void CreateRenderPass();
        void CreateFramebuffers();

        void CreateCommandPool();
        void CreateCommandBuffer();
        void CreateSyncObjects();

        void CleanupSwapChain();
        void CleanupImageViews();
        void CleanupFramebuffers();

        void RecreateSwapchain();

        void CreateViewport();

        uint32_t RatePhysicalDevice(VkPhysicalDevice device);
        void SelectPhysicalDevice();
        bool IsDeviceSuitable(VkPhysicalDevice device);
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        VKSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, HWND window);

        StWindowHandle m_WindowHandle;
        bool m_VSync;

        VkInstance m_Instance;
        VkSurfaceKHR m_Surface;
        VkPhysicalDevice m_CurrentDevice;
        VkDevice m_LogicalDevice;
        VkDescriptorPool m_DescriptorPool;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;

        VkSwapchainKHR m_SwapChain;
        std::vector<VkImage> m_SwapChainImages;
        VkFormat m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;
        uint32_t m_ImageCount;
        uint32_t m_MinImageCount;

        VkSemaphore m_AvailableSemaphore;
        VkSemaphore m_FinishedSemaphore;
        VkFence m_Fence;

        std::vector<VkImageView> m_SwapChainImageViews;
        std::vector<VkFramebuffer> m_SwapChainFramebuffers;

        VkRenderPass m_RenderPass;
        VkCommandPool m_CommandPool;
        VkCommandBuffer m_CommandBuffer;

        VkPhysicalDeviceProperties m_DeviceProperties;
        std::multimap<uint32_t, VkPhysicalDevice> m_Devices;

        uint32_t m_GraphicsQueueFamilyIndex;
        uint32_t m_PresentQueueFamilyIndex;

        VkViewport m_Viewport;
        VkRect2D m_ScissorRect;

#if defined(DEBUG) || defined(_DEBUG)
        VkDebugUtilsMessengerEXT m_Debugger;
        PFN_vkCreateDebugUtilsMessengerEXT m_DebuggerCreator;
        PFN_vkDestroyDebugUtilsMessengerEXT m_DebuggerDestroyer;

        const std::vector<const char*> m_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        VKSwapChainSupportDetails m_SwapChainDetails;
#endif

        VKFrame m_CurrentFrame;

        const std::vector<const char*> m_DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        float m_ClearColor[4];

        uint32_t m_MaxFramesInFlight;
    };
}


#endif //VK_CONTEXT_HPP
