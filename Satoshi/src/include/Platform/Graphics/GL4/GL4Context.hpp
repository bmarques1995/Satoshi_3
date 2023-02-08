#ifndef GL4_CONTEXT_HPP
#define GL4_CONTEXT_HPP

#include "Satoshi/Renderer/GraphicsContext.hpp"

#ifdef ST_PLATFORM_WINDOWS
#include <glad/wgl.h>
#endif

namespace Satoshi
{
    class GL4Context : public GraphicsContext
    {
    public:
        GL4Context(StWindowHandle window, uint32_t width, uint32_t height);
        ~GL4Context();

        virtual void ClearTarget() override;
        virtual void SetClearColor(float r, float g, float b, float a) override;
        virtual void ReceiveCommands() override;
        virtual void DispatchCommands() override;
        virtual void Draw(uint32_t elements) override;
        virtual void NewFrame() override;
        virtual void EndFrame() override;
        virtual void Present() override;

        virtual bool IsVSync() override;
        virtual void SetVSync(bool isVSync) override;

        virtual void GetGPUName(std::string* output) override;

        virtual void OnResize(WindowResizeEvent& e) override;

    private:
        float m_ClearColor[4];
        bool m_VSync;

        void CreateDevice();
        void CreateDescriptor();
        void CreateContext();
        void CreateViewport();

        StWindowHandle m_WindowHandle;
        uint32_t m_Width;
        uint32_t m_Height;

#ifdef ST_PLATFORM_WINDOWS
        HDC m_HDC;
        HGLRC m_HRC;
        PFNWGLSWAPINTERVALEXTPROC SwapIntervalEXT;
#endif
    };
}


#endif //GL4_CONTEXT_HPP
