#ifdef ST_PLATFORM_WINDOWS

#ifndef D3D12_SHADER_HPP
#define D3D12_SHADER_HPP

#include "Satoshi/Renderer/Shader.hpp"
#include "Platform/Graphics/D3D12/D3D12Context.hpp"

namespace Satoshi
{
	class D3D12Shader : public Shader
	{
	public:
		D3D12Shader(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements);
		~D3D12Shader();

		virtual void Stage() const override;
		virtual const uint32_t GetOffset() const override;

		virtual const BufferLayout& GetLayout() const override;

	private:

		ComPtr<ID3D12PipelineState> m_GraphicsPipeline;
		ComPtr<ID3D12RootSignature> m_RootSignature;

		void ReadBlob(std::string_view baseShaderPath, SHADER_VERSION shaderVersion, GRAPHICS_SHADER_KIND shaderKind, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc, ComPtr<ID3DBlob>* shaderBlob);
		void RegisterShaderBuilders();

		void BuildBlender(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);
		void BuildRasterizer(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);
		void BuildDepthStencil(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);
		void CreateGraphicsRootSignature(ID3D12RootSignature** rootSignature, ID3D12Device* device);

		void RegisterVertexShader(std::string_view baseShaderPath, SHADER_VERSION shaderVersion, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc, ComPtr<ID3DBlob>* shaderBlob);
		void RegisterPixelShader(std::string_view baseShaderPath, SHADER_VERSION shaderVersion, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc, ComPtr<ID3DBlob>* shaderBlob);
		void RegisterGeometryShader(std::string_view baseShaderPath, SHADER_VERSION shaderVersion, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc, ComPtr<ID3DBlob>* shaderBlob);
		void RegisterHullShader(std::string_view baseShaderPath, SHADER_VERSION shaderVersion, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc, ComPtr<ID3DBlob>* shaderBlob);
		void RegisterDomainShader(std::string_view baseShaderPath, SHADER_VERSION shaderVersion, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc, ComPtr<ID3DBlob>* shaderBlob);

		DXGI_FORMAT GetFormat(ShaderDataType type);

		std::unordered_map<GRAPHICS_SHADER_KIND, std::function<void(std::string_view, SHADER_VERSION, D3D12_GRAPHICS_PIPELINE_STATE_DESC*, ComPtr<ID3DBlob>*)>> m_ShaderRegisters;

		BufferLayout m_Layout;
		ShaderGroup m_ShaderGroup;
		ID3D12GraphicsCommandList* m_CommandList;
	};

}

#endif //D3D12_SHADER_HPP

#endif
