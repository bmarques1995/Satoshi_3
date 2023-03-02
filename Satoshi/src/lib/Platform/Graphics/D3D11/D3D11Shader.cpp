#include "Platform/Graphics/D3D11/D3D11Shader.hpp"
#include "Satoshi.hpp"

Satoshi::D3D11Shader::D3D11Shader(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements) :
    m_Layout(elements), m_ShaderGroup(shaderGroup)
{
    D3D11Data d3d11Data = std::any_cast<D3D11Data>(Application::GetInstance()->GetContext()->GetContextRunners());
    m_Context = d3d11Data.DeviceContext;
    CreateShaders(shaderGroup, d3d11Data.Device);
    CreateInputLayout(shaderGroup, d3d11Data.Device);
}

Satoshi::D3D11Shader::~D3D11Shader()
{
}

void Satoshi::D3D11Shader::Stage() const
{
    m_Context->IASetInputLayout(m_InputLayout.Get());
    for (size_t i = 0; i < m_ShaderGroup.GetShaderDetails().size(); ++i)
    {
        m_ShaderStagers[m_ShaderGroup.GetShaderDetails()[i].ShaderKind](m_Context);
    }
}

const uint32_t Satoshi::D3D11Shader::GetOffset() const
{
    return 0;
}

const Satoshi::BufferLayout& Satoshi::D3D11Shader::GetLayout() const
{
    return m_Layout;
}

void Satoshi::D3D11Shader::CreateShaders(const ShaderGroup& shaderGroup, ID3D11Device* device)
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

void Satoshi::D3D11Shader::CreateInputLayout(const ShaderGroup& shaderGroup, ID3D11Device* device)
{
    HRESULT hr;

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
    device->CreateInputLayout(ied, details.size(), m_VertexBlob->GetBufferPointer(), m_VertexBlob->GetBufferSize(), m_InputLayout.GetAddressOf());

    delete[] ied;
}

void Satoshi::D3D11Shader::RegisterShaderBuilders()
{
    m_ShaderBuilders[SHADER_KIND::SHADER_KIND_VERTEX] = std::bind(&D3D11Shader::BuildVertexShader, this, std::placeholders::_1, std::placeholders::_2);
    m_ShaderBuilders[SHADER_KIND::SHADER_KIND_PIXEL] = std::bind(&D3D11Shader::BuildPixelShader, this, std::placeholders::_1, std::placeholders::_2);
    m_ShaderBuilders[SHADER_KIND::SHADER_KIND_GEOMETRY] = std::bind(&D3D11Shader::BuildGeometryShader, this, std::placeholders::_1, std::placeholders::_2);
    m_ShaderBuilders[SHADER_KIND::SHADER_KIND_HULL] = std::bind(&D3D11Shader::BuildHullShader, this, std::placeholders::_1, std::placeholders::_2);
    m_ShaderBuilders[SHADER_KIND::SHADER_KIND_DOMAIN] = std::bind(&D3D11Shader::BuildDomainShader, this, std::placeholders::_1, std::placeholders::_2);
    m_ShaderBuilders[SHADER_KIND::SHADER_KIND_COMPUTE] = std::bind(&D3D11Shader::BuildComputeShader, this, std::placeholders::_1, std::placeholders::_2);
}

void Satoshi::D3D11Shader::RegisterShaderStagers()
{
    m_ShaderStagers[SHADER_KIND::SHADER_KIND_VERTEX] = std::bind(&D3D11Shader::StageVertexShader, this, std::placeholders::_1);
    m_ShaderStagers[SHADER_KIND::SHADER_KIND_PIXEL] = std::bind(&D3D11Shader::StagePixelShader, this, std::placeholders::_1);
    m_ShaderStagers[SHADER_KIND::SHADER_KIND_GEOMETRY] = std::bind(&D3D11Shader::StageGeometryShader, this, std::placeholders::_1);
    m_ShaderStagers[SHADER_KIND::SHADER_KIND_HULL] = std::bind(&D3D11Shader::StageHullShader, this, std::placeholders::_1);
    m_ShaderStagers[SHADER_KIND::SHADER_KIND_DOMAIN] = std::bind(&D3D11Shader::StageDomainShader, this, std::placeholders::_1);
    m_ShaderStagers[SHADER_KIND::SHADER_KIND_COMPUTE] = std::bind(&D3D11Shader::StageComputeShader, this, std::placeholders::_1);
}

void Satoshi::D3D11Shader::BuildVertexShader(std::string_view blobPath, ID3D11Device* device)
{
    HRESULT hr;
    std::wstring w_Filepath = std::wstring(blobPath.begin(), blobPath.end());
    D3DReadFileToBlob(w_Filepath.c_str(), m_VertexBlob.GetAddressOf());
    hr = device->CreateVertexShader(m_VertexBlob->GetBufferPointer(), m_VertexBlob->GetBufferSize(), nullptr, m_VertexShader.GetAddressOf());
    assert(hr == S_OK);
}

void Satoshi::D3D11Shader::BuildPixelShader(std::string_view blobPath, ID3D11Device* device)
{
    HRESULT hr;
    std::wstring w_Filepath = std::wstring(blobPath.begin(), blobPath.end());
    D3DReadFileToBlob(w_Filepath.c_str(), m_PixelBlob.GetAddressOf());
    hr = device->CreatePixelShader(m_PixelBlob->GetBufferPointer(), m_PixelBlob->GetBufferSize(), nullptr, m_PixelShader.GetAddressOf());
    assert(hr == S_OK);
}

void Satoshi::D3D11Shader::BuildGeometryShader(std::string_view blobPath, ID3D11Device* device)
{
    HRESULT hr;
    std::wstring w_Filepath = std::wstring(blobPath.begin(), blobPath.end());
    D3DReadFileToBlob(w_Filepath.c_str(), m_GeometryBlob.GetAddressOf());
    hr = device->CreateGeometryShader(m_GeometryBlob->GetBufferPointer(), m_GeometryBlob->GetBufferSize(), nullptr, m_GeometryShader.GetAddressOf());
    assert(hr == S_OK);
}

void Satoshi::D3D11Shader::BuildHullShader(std::string_view blobPath, ID3D11Device* device)
{
    HRESULT hr;
    std::wstring w_Filepath = std::wstring(blobPath.begin(), blobPath.end());
    D3DReadFileToBlob(w_Filepath.c_str(), m_HullBlob.GetAddressOf());
    hr = device->CreateHullShader(m_HullBlob->GetBufferPointer(), m_HullBlob->GetBufferSize(), nullptr, m_HullShader.GetAddressOf());
    assert(hr == S_OK);
}

void Satoshi::D3D11Shader::BuildDomainShader(std::string_view blobPath, ID3D11Device* device)
{
    HRESULT hr;
    std::wstring w_Filepath = std::wstring(blobPath.begin(), blobPath.end());
    D3DReadFileToBlob(w_Filepath.c_str(), m_DomainBlob.GetAddressOf());
    hr = device->CreateDomainShader(m_DomainBlob->GetBufferPointer(), m_DomainBlob->GetBufferSize(), nullptr, m_DomainShader.GetAddressOf());
    assert(hr == S_OK);
}

void Satoshi::D3D11Shader::BuildComputeShader(std::string_view blobPath, ID3D11Device* device)
{
    HRESULT hr;
    std::wstring w_Filepath = std::wstring(blobPath.begin(), blobPath.end());
    D3DReadFileToBlob(w_Filepath.data(), m_ComputeBlob.GetAddressOf());
    hr = device->CreateComputeShader(m_ComputeBlob->GetBufferPointer(), m_ComputeBlob->GetBufferSize(), nullptr, m_ComputeShader.GetAddressOf());
    assert(hr == S_OK);
}

void Satoshi::D3D11Shader::StageVertexShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
}

void Satoshi::D3D11Shader::StagePixelShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);
}

void Satoshi::D3D11Shader::StageGeometryShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->GSSetShader(m_GeometryShader.Get(), nullptr, 0);
}

void Satoshi::D3D11Shader::StageHullShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->HSSetShader(m_HullShader.Get(), nullptr, 0);
}

void Satoshi::D3D11Shader::StageDomainShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->DSSetShader(m_DomainShader.Get(), nullptr, 0);
}

void Satoshi::D3D11Shader::StageComputeShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->CSSetShader(m_ComputeShader.Get(), nullptr, 0);
}

DXGI_FORMAT Satoshi::D3D11Shader::GetFormat(ShaderDataType type)
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
