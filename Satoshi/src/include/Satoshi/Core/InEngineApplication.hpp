#ifndef IN_ENGINE_APPLICATION_HPP
#define IN_ENGINE_APPLICATION_HPP

#include "Core.hpp"
#include <stpch.hpp>

#include "Satoshi/Renderer/RendererAPI.hpp"
#include "Satoshi/Renderer/GraphicsContext.hpp"

namespace Satoshi
{
    class SATOSHI_API InEngineApplication
    {
    public:
        InEngineApplication(StWindowHandle parent, uint32_t width, uint32_t height);
        ~InEngineApplication();

        StWindowHandle GetNativeWindow();

        static InEngineApplication* GetInstance();

        uint32_t GetWidth();
        uint32_t GetHeight();

        void Update();
    private:
        StWindowHandle m_WindowHandle;
        uint32_t m_Width;
        uint32_t m_Height;

        static InEngineApplication* s_Instance;

        std::unique_ptr<GraphicsContext> m_Context;
        GRAPHICS_API m_API;
    };
}


#endif //IN_ENGINE_APPLICATION_HPP