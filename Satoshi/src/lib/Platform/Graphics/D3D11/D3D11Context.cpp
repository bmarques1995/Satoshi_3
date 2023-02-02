#ifdef ST_PLATFORM_WINDOWS

#include "Platform/Graphics/D3D11/D3D11Context.hpp"
#include <cassert>
#include "Satoshi/Core/Application.hpp"

Satoshi::D3D11Context::D3D11Context()
{
    m_ClearColor[0] = .6f;
    m_ClearColor[1] = 1.0f;
    m_ClearColor[2] = .3f;
    m_ClearColor[3] = 1.0f;

    CreateDeviceAndSwapchain();
    CreateAdapter();
    CreateRenderTarget();
    CreateViewport(Application::GetInstance()->GetWidth(), Application::GetInstance()->GetHeight());
}

Satoshi::D3D11Context::~D3D11Context()
{
}

void Satoshi::D3D11Context::Present()
{
    m_SwapChain->Present(1, 0);
}

void Satoshi::D3D11Context::OnResize(WindowResizeEvent& e)
{
    m_RenderTargetView.Reset();
    m_SwapChain->ResizeBuffers(0, e.GetWidth(), e.GetHeight(), DXGI_FORMAT_UNKNOWN, 0);
    CreateRenderTarget();
    CreateViewport(e.GetWidth(), e.GetHeight());
}

void Satoshi::D3D11Context::ClearTarget()
{
    m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), NULL);
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), m_ClearColor);
}

void Satoshi::D3D11Context::SetClearColor(float r, float g, float b, float a)
{
    m_ClearColor[0] = r;
    m_ClearColor[1] = g;
    m_ClearColor[2] = b;
    m_ClearColor[3] = a;
}

void Satoshi::D3D11Context::ReceiveCommands()
{
}

void Satoshi::D3D11Context::DispatchCommands()
{
}

void Satoshi::D3D11Context::Draw(uint32_t elements)
{
}

void Satoshi::D3D11Context::NewFrame()
{
}

void Satoshi::D3D11Context::EndFrame()
{
}

void Satoshi::D3D11Context::CreateDeviceAndSwapchain()
{
    D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_11_1;
    UINT flags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = std::any_cast<HWND>(Application::GetInstance()->GetNativeWindow());
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    HRESULT hr;
    hr = D3D11CreateDeviceAndSwapChain
    (
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        flags,
        NULL,
        0,
        D3D11_SDK_VERSION,
        &sd,
        m_SwapChain.GetAddressOf(),
        m_Device.GetAddressOf(),
        &fl,
        m_DeviceContext.GetAddressOf()
    );

    assert(hr == S_OK);
}

void Satoshi::D3D11Context::CreateAdapter()
{
    IDXGIFactory4* dxgiFactory = nullptr;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    
    hr = dxgiFactory->EnumAdapters1(0, m_Adapter.GetAddressOf());
    assert(hr == S_OK);

#if 0
    auto adapterDescription = DXGI_ADAPTER_DESC1();
    m_Adapter->GetDesc1(&adapterDescription);
#endif

    dxgiFactory->Release();
}

void Satoshi::D3D11Context::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    HRESULT hr = m_Device->CreateRenderTargetView(pBackBuffer, NULL, m_RenderTargetView.GetAddressOf());
    assert(hr == S_OK);
    pBackBuffer->Release();
}

void Satoshi::D3D11Context::CreateViewport(uint32_t width, uint32_t height)
{
    m_ScissorRect.left = m_Viewport.TopLeftX = 0;
    m_ScissorRect.top = m_Viewport.TopLeftY = 0;
    m_ScissorRect.right = m_Viewport.Width = (float) width;
    m_ScissorRect.bottom = m_Viewport.Height = (float) height;
    m_Viewport.MinDepth = .0f;
    m_Viewport.MaxDepth = 1.0f;
}

#endif
