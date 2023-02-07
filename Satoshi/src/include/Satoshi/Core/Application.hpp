#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Core.hpp"
#include <stpch.hpp>

#include "Satoshi/Renderer/RendererAPI.hpp"

#ifdef ST_PLATFORM_WINDOWS
#include <windows.h>

typedef HWND StWindowHandle;
#else
#error API not implemented
#endif

#include "Satoshi/Renderer/GraphicsContext.hpp"

namespace Satoshi
{
    class SATOSHI_API Application
    {
    public:
        Application(StWindowHandle parent, uint32_t width, uint32_t height);
        ~Application();

        StWindowHandle GetNativeWindow();

        static Application* GetInstance();

        uint32_t GetWidth();
        uint32_t GetHeight();

        void Update();
    private:
        StWindowHandle m_WindowHandle;
        uint32_t m_Width;
        uint32_t m_Height;

        static Application* s_Instance;

        std::unique_ptr<GraphicsContext> m_Context;
        GRAPHICS_API m_API;
    };
}


#endif //APPLICATION_HPP
