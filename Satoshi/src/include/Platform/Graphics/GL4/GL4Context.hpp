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
        GL4Context();
        ~GL4Context();

        virtual void ClearTarget() override;
        virtual void SetClearColor(float r, float g, float b, float a) override;
        virtual void ReceiveCommands() override;
        virtual void DispatchCommands() override;
        virtual void Draw(uint32_t elements) override;
        virtual void NewFrame() override;
        virtual void EndFrame() override;
        virtual void Present() override;

        virtual void OnResize(WindowResizeEvent& e) override;

    private:
        float m_ClearColor[4];

#ifdef ST_PLATFORM_WINDOWS
        HDC m_HDC;
        HGLRC m_HRC;
        PFNWGLSWAPINTERVALEXTPROC SwapIntervalEXT;
#endif
    };
}


#endif //GL4_CONTEXT_HPP
