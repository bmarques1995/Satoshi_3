#include "Satoshi/Core/ImplLayer.hpp"

#include "Satoshi.hpp"

Satoshi::ImplLayer::ImplLayer(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements) :
    m_Layout(elements), m_ShaderGroup(shaderGroup)
{
    D3D11Data d3d11Data = std::any_cast<D3D11Data>(Application::GetInstance()->GetContext()->GetContextRunners());
    m_Context = d3d11Data.DeviceContext;
    CreateShaders(shaderGroup, d3d11Data.Device);
    CreateInputLayout(shaderGroup,d3d11Data.Device);
    CreateVertexBuffer((const void*)vBuffer, sizeof(vBuffer), m_Layout.GetStride(), d3d11Data.Device);
    CreateIndexBuffer((const void*)iBuffer, sizeof(iBuffer) / sizeof(uint32_t),d3d11Data.Device);
}

void Satoshi::ImplLayer::OnAttach()
{
}

void Satoshi::ImplLayer::OnDetach()
{
}

void Satoshi::ImplLayer::OnUpdate()
{
	if (Input::IsKeyPressed(ST_KEY_C))
		Console::Log("Key C was pressed");
    UINT offset = 0;
    m_Context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_VBStride, &offset);
    m_Context->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    m_Context->IASetInputLayout(m_InputLayout.Get());
    std::vector<ShaderDetails> details = m_ShaderGroup.GetShaderDetails();
    for (size_t i = 0; i < details.size(); ++i)
    {
        m_ShaderStagers[details[i].ShaderKind](m_Context);
    }
}

void Satoshi::ImplLayer::OnEvent(Event& event)
{
}

void Satoshi::ImplLayer::CreateShaders(const ShaderGroup& shaderGroup, ID3D11Device* device)
{
    RegisterShaderBuilders();
    RegisterShaderStagers();
    auto details = shaderGroup.GetShaderDetails();
    auto shaderManager = Application::GetInstance()->GetShaderManager();
    for (size_t i = 0; i < details.size(); i++)
    {
        m_ShaderBuilders[details[i].ShaderKind](shaderManager->BuildBlobFilename(shaderGroup.GetBasePath(), details[i].ShaderKind, m_ShaderGroup.GetShaderVersion()), device);
    }
}

void Satoshi::ImplLayer::CreateInputLayout(const ShaderGroup& shaderGroup, ID3D11Device* device)
{
    HRESULT hr;

    D3D11Data d3d11Data = std::any_cast<D3D11Data>(Application::GetInstance()->GetContext()->GetContextRunners());

    auto nativeElements = m_Layout.GetElements();
    D3D11_INPUT_ELEMENT_DESC* ied = new D3D11_INPUT_ELEMENT_DESC[nativeElements.size()];

    for (size_t i = 0; i < nativeElements.size(); i++)
    {
        ied[i].SemanticName = nativeElements[i].Name.c_str();
        ied[i].SemanticIndex = 0;
        ied[i].Format = GetFormat(nativeElements[i].Type);
        ied[i].InputSlot = 0;
        ied[i].AlignedByteOffset = nativeElements[i].Offset;
        ied[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        ied[i].InstanceDataStepRate = 0;
    }

    auto details = shaderGroup.GetShaderDetails();
    d3d11Data.Device->CreateInputLayout(ied, details.size(), m_VertexBlob->GetBufferPointer(), m_VertexBlob->GetBufferSize(), m_InputLayout.GetAddressOf());

    delete[] ied;
}

void Satoshi::ImplLayer::CreateVertexBuffer(const void* data, size_t size, uint32_t stride, ID3D11Device* device)
{
    HRESULT hr;
    m_VBStride = stride;

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

    hr = device->CreateBuffer(&bd, &initData, m_VertexBuffer.GetAddressOf());
    assert(hr == S_OK);
}

void Satoshi::ImplLayer::CreateIndexBuffer(const void* data, size_t count, ID3D11Device* device)
{
    HRESULT hr;
    m_IBCount = count;

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

    hr = device->CreateBuffer(&bd, &initData, m_IndexBuffer.GetAddressOf());
    assert(hr == S_OK);
}

void Satoshi::ImplLayer::RegisterShaderBuilders()
{
    m_ShaderBuilders[SHADER_KIND::SHADER_KIND_VERTEX] = std::bind(&ImplLayer::BuildVertexShader, this, std::placeholders::_1, std::placeholders::_2);
    m_ShaderBuilders[SHADER_KIND::SHADER_KIND_PIXEL] = std::bind(&ImplLayer::BuildPixelShader, this, std::placeholders::_1, std::placeholders::_2);
    m_ShaderBuilders[SHADER_KIND::SHADER_KIND_GEOMETRY] = std::bind(&ImplLayer::BuildGeometryShader, this, std::placeholders::_1, std::placeholders::_2);
    m_ShaderBuilders[SHADER_KIND::SHADER_KIND_HULL] = std::bind(&ImplLayer::BuildHullShader, this, std::placeholders::_1, std::placeholders::_2);
    m_ShaderBuilders[SHADER_KIND::SHADER_KIND_DOMAIN] = std::bind(&ImplLayer::BuildDomainShader, this, std::placeholders::_1, std::placeholders::_2);
    m_ShaderBuilders[SHADER_KIND::SHADER_KIND_COMPUTE] = std::bind(&ImplLayer::BuildComputeShader, this, std::placeholders::_1, std::placeholders::_2);
}

void Satoshi::ImplLayer::RegisterShaderStagers()
{
    m_ShaderStagers[SHADER_KIND::SHADER_KIND_VERTEX] = std::bind(&ImplLayer::StageVertexShader, this, std::placeholders::_1);
    m_ShaderStagers[SHADER_KIND::SHADER_KIND_PIXEL] = std::bind(&ImplLayer::StagePixelShader, this, std::placeholders::_1);
    m_ShaderStagers[SHADER_KIND::SHADER_KIND_GEOMETRY] = std::bind(&ImplLayer::StageGeometryShader, this, std::placeholders::_1);
    m_ShaderStagers[SHADER_KIND::SHADER_KIND_HULL] = std::bind(&ImplLayer::StageHullShader, this, std::placeholders::_1);
    m_ShaderStagers[SHADER_KIND::SHADER_KIND_DOMAIN] = std::bind(&ImplLayer::StageDomainShader, this, std::placeholders::_1);
    m_ShaderStagers[SHADER_KIND::SHADER_KIND_COMPUTE] = std::bind(&ImplLayer::StageComputeShader, this, std::placeholders::_1);
}

void Satoshi::ImplLayer::BuildVertexShader(std::string_view blobPath, ID3D11Device* device)
{
    HRESULT hr;
    std::wstring w_Filepath = std::wstring(blobPath.begin(), blobPath.end());
    D3DReadFileToBlob(w_Filepath.c_str(), m_VertexBlob.GetAddressOf());
    Console::Log("{0} {1}", m_VertexBlob->GetBufferPointer(), m_VertexBlob->GetBufferSize());
    hr = device->CreateVertexShader(m_VertexBlob->GetBufferPointer(), m_VertexBlob->GetBufferSize(), nullptr, m_VertexShader.GetAddressOf());
    assert(hr == S_OK);
}

void Satoshi::ImplLayer::BuildPixelShader(std::string_view blobPath, ID3D11Device* device)
{
    HRESULT hr;
    std::wstring w_Filepath = std::wstring(blobPath.begin(), blobPath.end());
    D3DReadFileToBlob(w_Filepath.c_str(), m_PixelBlob.GetAddressOf());
    hr = device->CreatePixelShader(m_PixelBlob->GetBufferPointer(), m_PixelBlob->GetBufferSize(), nullptr, m_PixelShader.GetAddressOf());
    assert(hr == S_OK);
}

void Satoshi::ImplLayer::BuildGeometryShader(std::string_view blobPath, ID3D11Device* device)
{
    HRESULT hr;
    std::wstring w_Filepath = std::wstring(blobPath.begin(), blobPath.end());
    D3DReadFileToBlob(w_Filepath.c_str(), m_GeometryBlob.GetAddressOf());
    hr = device->CreateGeometryShader(m_GeometryBlob->GetBufferPointer(), m_GeometryBlob->GetBufferSize(), nullptr, m_GeometryShader.GetAddressOf());
    assert(hr == S_OK);
}

void Satoshi::ImplLayer::BuildHullShader(std::string_view blobPath, ID3D11Device* device)
{
    HRESULT hr;
    std::wstring w_Filepath = std::wstring(blobPath.begin(), blobPath.end());
    D3DReadFileToBlob(w_Filepath.c_str(), m_HullBlob.GetAddressOf());
    hr = device->CreateHullShader(m_HullBlob->GetBufferPointer(), m_HullBlob->GetBufferSize(), nullptr, m_HullShader.GetAddressOf());
    assert(hr == S_OK);
}

void Satoshi::ImplLayer::BuildDomainShader(std::string_view blobPath, ID3D11Device* device)
{
    HRESULT hr;
    std::wstring w_Filepath = std::wstring(blobPath.begin(), blobPath.end());
    D3DReadFileToBlob(w_Filepath.c_str(), m_DomainBlob.GetAddressOf());
    hr = device->CreateDomainShader(m_DomainBlob->GetBufferPointer(), m_DomainBlob->GetBufferSize(), nullptr, m_DomainShader.GetAddressOf());
    assert(hr == S_OK);
}

void Satoshi::ImplLayer::BuildComputeShader(std::string_view blobPath, ID3D11Device* device)
{
    HRESULT hr;
    std::wstring w_Filepath = std::wstring(blobPath.begin(), blobPath.end());
    D3DReadFileToBlob(w_Filepath.data(), m_ComputeBlob.GetAddressOf());
    hr = device->CreateComputeShader(m_ComputeBlob->GetBufferPointer(), m_ComputeBlob->GetBufferSize(), nullptr, m_ComputeShader.GetAddressOf());
    assert(hr == S_OK);
}

void Satoshi::ImplLayer::StageVertexShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
}

void Satoshi::ImplLayer::StagePixelShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);
}

void Satoshi::ImplLayer::StageGeometryShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->GSSetShader(m_GeometryShader.Get(), nullptr, 0);
}

void Satoshi::ImplLayer::StageHullShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->HSSetShader(m_HullShader.Get(), nullptr, 0);
}

void Satoshi::ImplLayer::StageDomainShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->DSSetShader(m_DomainShader.Get(), nullptr, 0);
}

void Satoshi::ImplLayer::StageComputeShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->CSSetShader(m_ComputeShader.Get(), nullptr, 0);
}

DXGI_FORMAT Satoshi::ImplLayer::GetFormat(ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float: return DXGI_FORMAT_R32_FLOAT;
    case ShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
    case ShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
    case ShaderDataType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case ShaderDataType::Uint: return DXGI_FORMAT_R32_UINT;
    case ShaderDataType::Uint2: return DXGI_FORMAT_R32G32_UINT;
    case ShaderDataType::Uint3: return DXGI_FORMAT_R32G32B32_UINT;
    case ShaderDataType::Uint4: return DXGI_FORMAT_R32G32B32A32_UINT;
    case ShaderDataType::Bool: return DXGI_FORMAT_R8_UINT;
    default: return DXGI_FORMAT_UNKNOWN;
    }
}
