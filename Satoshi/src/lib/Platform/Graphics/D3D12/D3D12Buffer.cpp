#ifdef ST_PLATFORM_WINDOWS

#include "Platform/Graphics/D3D12/D3D12Buffer.hpp"
#include "Satoshi.hpp"

void Satoshi::D3D12Buffer::CreateBuffer(const void* data, size_t size, ID3D12Device* device, D3D12MA::Allocator* allocator)
{
    HRESULT hr;

    D3D12_RESOURCE_DESC bufferDesc = {};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = size;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12MA::ALLOCATION_DESC allocDesc = {};
    allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

    allocator->CreateResource(&allocDesc, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, m_Allocation.GetAddressOf(), IID_PPV_ARGS(m_Buffer.GetAddressOf()));

    // Copy vertex data to buffer
    void* rawData;
    m_Buffer->Map(0, nullptr, &rawData);
    memcpy(rawData, data, bufferDesc.Width);
    m_Buffer->Unmap(0, nullptr);
}

Satoshi::D3D12VertexBuffer::D3D12VertexBuffer(const void* data, size_t size, uint32_t stride)
{
    D3D12Data d3d12Data = std::any_cast<D3D12Data>(Application::GetInstance()->GetContext()->GetContextRunners());
    m_CommandList = d3d12Data.CommandList;
    CreateBuffer(data, size, d3d12Data.Device, d3d12Data.Allocator);
    m_Stride = stride;

    memset(&m_VertexBufferView, 0, sizeof(D3D12_VERTEX_BUFFER_VIEW));
    m_VertexBufferView.BufferLocation = m_Buffer->GetGPUVirtualAddress();
    m_VertexBufferView.SizeInBytes = size;
    m_VertexBufferView.StrideInBytes = m_Stride;
}

Satoshi::D3D12VertexBuffer::~D3D12VertexBuffer()
{
}

void Satoshi::D3D12VertexBuffer::Stage() const
{
    m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
}

void Satoshi::D3D12VertexBuffer::RegisterLayout(IndexBuffer* indexBuffer)
{
}

Satoshi::D3D12IndexBuffer::D3D12IndexBuffer(const void* data, size_t count)
{
    D3D12Data d3d12Data = std::any_cast<D3D12Data>(Application::GetInstance()->GetContext()->GetContextRunners());
    m_CommandList = d3d12Data.CommandList;
    CreateBuffer(data, count * sizeof(uint32_t), d3d12Data.Device, d3d12Data.Allocator);
    m_Count = (uint32_t)count;

    m_IndexBufferView.BufferLocation = m_Buffer->GetGPUVirtualAddress();
    m_IndexBufferView.SizeInBytes = count * sizeof(uint32_t);
    m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

Satoshi::D3D12IndexBuffer::~D3D12IndexBuffer()
{
}

void Satoshi::D3D12IndexBuffer::Stage() const
{
    m_CommandList->IASetIndexBuffer(&m_IndexBufferView);
}

uint32_t Satoshi::D3D12IndexBuffer::GetCount() const
{
    return m_Count;
}

#endif
