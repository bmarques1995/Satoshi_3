#ifdef ST_PLATFORM_WINDOWS

#ifndef D3D11_CONTEXT_HPP
#define D3D11_CONTEXT_HPP

#include "Satoshi/Renderer/GraphicsContext.hpp"
#include "D3D11ShaderManager.hpp"

#include <d3d11_4.h>

#include <wrl.h>

using Microsoft::WRL::ComPtr;

namespace Satoshi
{
    struct D3D11Data
    {
        ID3D11Device* Device;
        ID3D11DeviceContext* DeviceContext;
    };
    class D3D11Context : public GraphicsContext
    {
    public:
        D3D11Context(StWindowHandle window, uint32_t width, uint32_t height);
        ~D3D11Context();

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
        void CreateDeviceAndSwapchain(StWindowHandle window);
        void CreateAdapter();
        void CreateRenderTarget();
        void CreateViewport(uint32_t width, uint32_t height);

        float m_ClearColor[4];
        bool m_VSync;

        ComPtr<ID3D11Device> m_Device;
        ComPtr<ID3D11DeviceContext> m_DeviceContext;
        ComPtr<IDXGISwapChain> m_SwapChain;
        ComPtr<IDXGIAdapter> m_Adapter1;
        ComPtr<ID3D11RenderTargetView> m_RenderTargetView;

        D3D11_VIEWPORT m_Viewport;
        D3D11_RECT m_ScissorRect;
    };
}


#endif //D3D11_CONTEXT_HPP

#endif //ST_PLATFORM_WINDOWS
