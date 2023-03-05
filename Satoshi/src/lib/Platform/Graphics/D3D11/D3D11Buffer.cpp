#ifdef ST_PLATFORM_WINDOWS

#include "Platform/Graphics/D3D11/D3D11Buffer.hpp"
#include "Satoshi.hpp"

Satoshi::D3D11VertexBuffer::D3D11VertexBuffer(const void* data, size_t size, uint32_t stride)
{
    HRESULT hr;
    m_Stride = stride;

    D3D11Data d3d11Data = std::any_cast<D3D11Data>(Application::GetInstance()->GetContext()->GetContextRunners());
    m_Context = d3d11Data.DeviceContext;

    D3D11_BUFFER_DESC bd;
    D3D11_SUBRESOURCE_DATA initData;

    ZeroMemory(&bd, sizeof(bd));
    ZeroMemory(&initData, sizeof(initData));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = (UINT)(size);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    initData.pSysMem = data;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    hr = d3d11Data.Device->CreateBuffer(&bd, &initData, m_VertexBuffer.GetAddressOf());
    assert(hr == S_OK);
}

Satoshi::D3D11VertexBuffer::~D3D11VertexBuffer()
{
}

void Satoshi::D3D11VertexBuffer::Stage() const
{
    uint32_t offset = 0;
    m_Context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &offset);
}

void Satoshi::D3D11VertexBuffer::RegisterLayout(IndexBuffer* indexBuffer)
{
}

Satoshi::D3D11IndexBuffer::D3D11IndexBuffer(const void* data, size_t count)
{
    HRESULT hr;
    m_Count = count;

    D3D11Data d3d11Data = std::any_cast<D3D11Data>(Application::GetInstance()->GetContext()->GetContextRunners());
    m_Context = d3d11Data.DeviceContext;

    D3D11_BUFFER_DESC bd;
    D3D11_SUBRESOURCE_DATA initData;

    ZeroMemory(&bd, sizeof(bd));
    ZeroMemory(&initData, sizeof(initData));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = (UINT)(count * sizeof(uint32_t));
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    initData.pSysMem = data;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    hr = d3d11Data.Device->CreateBuffer(&bd, &initData, m_IndexBuffer.GetAddressOf());
    assert(hr == S_OK);
}

Satoshi::D3D11IndexBuffer::~D3D11IndexBuffer()
{
}

void Satoshi::D3D11IndexBuffer::Stage() const
{
    m_Context->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

uint32_t Satoshi::D3D11IndexBuffer::GetCount() const
{
	return m_Count;
}

#endif
