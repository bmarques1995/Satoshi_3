#ifdef ST_PLATFORM_WINDOWS

#include "Platform/Graphics/D3D12/D3D12Shader.hpp"
#include "Satoshi.hpp"

Satoshi::D3D12Shader::D3D12Shader(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements) :
    m_ShaderGroup(shaderGroup), m_Layout(elements)
{
    HRESULT hr;

    RegisterShaderBuilders();
    D3D12Data d3d12Data = std::any_cast<D3D12Data>(Application::GetInstance()->GetContext()->GetContextRunners());
    m_CommandList = d3d12Data.CommandList;

    auto nativeElements = m_Layout.GetElements();
    D3D12_INPUT_ELEMENT_DESC* ied = new D3D12_INPUT_ELEMENT_DESC[elements.size()];

    for (size_t i = 0; i < nativeElements.size(); i++)
    {
        ied[i].SemanticName = nativeElements[i].Name.c_str();
        ied[i].SemanticIndex = 0;
        ied[i].Format = GetFormat(nativeElements[i].Type);
        ied[i].InputSlot = 0;
        ied[i].AlignedByteOffset = nativeElements[i].Offset;
        ied[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        ied[i].InstanceDataStepRate = 0;
    }

    CreateGraphicsRootSignature(&m_RootSignature, d3d12Data.Device);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsDesc = {};
    graphicsDesc.NodeMask = 1;
    graphicsDesc.InputLayout = { ied, (uint32_t)nativeElements.size() };
    graphicsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    graphicsDesc.pRootSignature = m_RootSignature.Get();
    graphicsDesc.SampleMask = UINT_MAX;
    graphicsDesc.NumRenderTargets = 1;
    graphicsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    graphicsDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
    //Pelo amor de Deus, não esquece de mim
    graphicsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
        graphicsDesc.SampleDesc.Count = 1;
    graphicsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    auto details = shaderGroup.GetShaderDetails();
    std::vector<ComPtr<ID3DBlob>> m_Blobs(details.size());
    for (size_t i = 0; i < details.size(); i++)
    {
        m_ShaderRegisters[details[i].ShaderKind](shaderGroup.GetBasePath(), shaderGroup.GetShaderVersion(), &graphicsDesc, &m_Blobs[i]);
    }

    BuildBlender(&graphicsDesc);
    BuildRasterizer(&graphicsDesc);
    BuildDepthStencil(&graphicsDesc);

    hr = d3d12Data.Device->CreateGraphicsPipelineState(&graphicsDesc, IID_PPV_ARGS(m_GraphicsPipeline.GetAddressOf()));
    assert(hr == S_OK);

    delete[] ied;
    m_Blobs.clear();
}

Satoshi::D3D12Shader::~D3D12Shader()
{
}

void Satoshi::D3D12Shader::Stage() const
{
    m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
    m_CommandList->SetPipelineState(m_GraphicsPipeline.Get());
}

const uint32_t Satoshi::D3D12Shader::GetOffset() const
{
    return 0;
}

const Satoshi::BufferLayout& Satoshi::D3D12Shader::GetLayout() const
{
    return m_Layout;
}

void Satoshi::D3D12Shader::ReadBlob(std::string_view baseShaderPath, SHADER_VERSION shaderVersion, SHADER_KIND shaderKind, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc, ComPtr<ID3DBlob>* shaderBlob)
{
    auto shaderManager = Application::GetInstance()->GetShaderManager();
    std::string_view filepath = shaderManager->BuildBlobFilename(baseShaderPath, shaderKind, shaderVersion);
    std::wstring w_Filepath = std::wstring(filepath.begin(), filepath.end());
    D3DReadFileToBlob(w_Filepath.c_str(), shaderBlob->GetAddressOf());
}

void Satoshi::D3D12Shader::RegisterShaderBuilders()
{
    m_ShaderRegisters[SHADER_KIND::SHADER_KIND_VERTEX] = std::bind(&D3D12Shader::RegisterVertexShader, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_ShaderRegisters[SHADER_KIND::SHADER_KIND_PIXEL] = std::bind(&D3D12Shader::RegisterPixelShader, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_ShaderRegisters[SHADER_KIND::SHADER_KIND_GEOMETRY] = std::bind(&D3D12Shader::RegisterGeometryShader, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_ShaderRegisters[SHADER_KIND::SHADER_KIND_HULL] = std::bind(&D3D12Shader::RegisterHullShader, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_ShaderRegisters[SHADER_KIND::SHADER_KIND_DOMAIN] = std::bind(&D3D12Shader::RegisterDomainShader, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    m_ShaderRegisters[SHADER_KIND::SHADER_KIND_COMPUTE] = std::bind(&D3D12Shader::RegisterComputeShader, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
}

void Satoshi::D3D12Shader::BuildBlender(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)
{
    graphicsDesc->BlendState.AlphaToCoverageEnable = false;
    graphicsDesc->BlendState.RenderTarget[0].BlendEnable = true;
    graphicsDesc->BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    graphicsDesc->BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    graphicsDesc->BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    graphicsDesc->BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    graphicsDesc->BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    graphicsDesc->BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    graphicsDesc->BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void Satoshi::D3D12Shader::BuildRasterizer(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)
{
    graphicsDesc->RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    graphicsDesc->RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    graphicsDesc->RasterizerState.FrontCounterClockwise = true;
    graphicsDesc->RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    graphicsDesc->RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    graphicsDesc->RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    graphicsDesc->RasterizerState.DepthClipEnable = true;
    graphicsDesc->RasterizerState.MultisampleEnable = false;
    graphicsDesc->RasterizerState.AntialiasedLineEnable = false;
    graphicsDesc->RasterizerState.ForcedSampleCount = 0;
    graphicsDesc->RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}

void Satoshi::D3D12Shader::BuildDepthStencil(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)
{
    graphicsDesc->DepthStencilState.DepthEnable = false;
    graphicsDesc->DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    graphicsDesc->DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    graphicsDesc->DepthStencilState.StencilEnable = false;
    graphicsDesc->DepthStencilState.FrontFace.StencilFailOp = graphicsDesc->DepthStencilState.FrontFace.StencilDepthFailOp = graphicsDesc->DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    graphicsDesc->DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    graphicsDesc->DepthStencilState.BackFace = graphicsDesc->DepthStencilState.FrontFace;
}

void Satoshi::D3D12Shader::CreateGraphicsRootSignature(ID3D12RootSignature** rootSignature, ID3D12Device* device)
{
    HRESULT hr;
    ID3DBlob* rootBlob;
    ID3DBlob* errorBlob;

    D3D12_DESCRIPTOR_RANGE descRange = {};

    descRange.BaseShaderRegister = 0;
    descRange.NumDescriptors = 1;
    descRange.OffsetInDescriptorsFromTableStart = 0;
    descRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descRange.RegisterSpace = 0;

    D3D12_ROOT_PARAMETER param[2] = {};

    param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    param[0].Constants.ShaderRegister = 0;
    param[0].Constants.RegisterSpace = 0;
    param[0].Constants.Num32BitValues = 16;
    param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    param[1].DescriptorTable.NumDescriptorRanges = 1;
    param[1].DescriptorTable.pDescriptorRanges = &descRange;
    param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_STATIC_SAMPLER_DESC staticSampler = {};
    staticSampler.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
    staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSampler.MipLODBias = .0f;
    staticSampler.MaxAnisotropy = 0;
    staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    staticSampler.MinLOD = .0f;
    staticSampler.MaxLOD = .0f;
    staticSampler.ShaderRegister = 0;
    staticSampler.RegisterSpace = 0;
    staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC desc = {};

    desc.NumParameters = sizeof(param) / sizeof(D3D12_ROOT_PARAMETER);
    desc.pParameters = param;
    desc.NumStaticSamplers = 1;
    desc.pStaticSamplers = &staticSampler;
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

    D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &rootBlob, &errorBlob);

    if (errorBlob != nullptr)
    {
        if (errorBlob->GetBufferSize())
        {
            OutputDebugStringA((const char*)errorBlob->GetBufferPointer());
            OutputDebugStringA("\n");
        }
        errorBlob->Release();
        assert(false);
    }

    hr = device->CreateRootSignature(0, rootBlob->GetBufferPointer(), rootBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature));
    assert(hr == S_OK);
}

void Satoshi::D3D12Shader::RegisterVertexShader(std::string_view baseShaderPath, SHADER_VERSION shaderVersion, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc, ComPtr<ID3DBlob>* shaderBlob)
{
    ReadBlob(baseShaderPath, shaderVersion, SHADER_KIND::SHADER_KIND_VERTEX, graphicsDesc, shaderBlob);
    graphicsDesc->VS = { (*shaderBlob)->GetBufferPointer(), (*shaderBlob)->GetBufferSize() };
}

void Satoshi::D3D12Shader::RegisterPixelShader(std::string_view baseShaderPath, SHADER_VERSION shaderVersion, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc, ComPtr<ID3DBlob>* shaderBlob)
{
    ReadBlob(baseShaderPath, shaderVersion, SHADER_KIND::SHADER_KIND_PIXEL, graphicsDesc, shaderBlob);
    graphicsDesc->PS = { (*shaderBlob)->GetBufferPointer(), (*shaderBlob)->GetBufferSize() };
}

void Satoshi::D3D12Shader::RegisterGeometryShader(std::string_view baseShaderPath, SHADER_VERSION shaderVersion, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc, ComPtr<ID3DBlob>* shaderBlob)
{
    ReadBlob(baseShaderPath, shaderVersion, SHADER_KIND::SHADER_KIND_GEOMETRY, graphicsDesc, shaderBlob);
    graphicsDesc->GS = { (*shaderBlob)->GetBufferPointer(), (*shaderBlob)->GetBufferSize() };
}

void Satoshi::D3D12Shader::RegisterHullShader(std::string_view baseShaderPath, SHADER_VERSION shaderVersion, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc, ComPtr<ID3DBlob>* shaderBlob)
{
    ReadBlob(baseShaderPath, shaderVersion, SHADER_KIND::SHADER_KIND_HULL, graphicsDesc, shaderBlob); 
    graphicsDesc->HS = { (*shaderBlob)->GetBufferPointer(), (*shaderBlob)->GetBufferSize() };
}

void Satoshi::D3D12Shader::RegisterDomainShader(std::string_view baseShaderPath, SHADER_VERSION shaderVersion, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc, ComPtr<ID3DBlob>* shaderBlob)
{
    ReadBlob(baseShaderPath, shaderVersion, SHADER_KIND::SHADER_KIND_DOMAIN, graphicsDesc, shaderBlob);
    graphicsDesc->DS = { (*shaderBlob)->GetBufferPointer(), (*shaderBlob)->GetBufferSize() };
}

void Satoshi::D3D12Shader::RegisterComputeShader(std::string_view baseShaderPath, SHADER_VERSION shaderVersion, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc, ComPtr<ID3DBlob>* shaderBlob)
{
}

DXGI_FORMAT Satoshi::D3D12Shader::GetFormat(ShaderDataType type)
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

#endif
