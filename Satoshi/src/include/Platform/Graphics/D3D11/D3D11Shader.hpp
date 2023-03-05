#ifdef ST_PLATFORM_WINDOWS

#ifndef D3D11_SHADER_HPP
#define D3D11_SHADER_HPP

#include "Satoshi/Renderer/Shader.hpp"
#include "Platform/Graphics/D3D11/D3D11Context.hpp"

namespace Satoshi
{
	class D3D11Shader : public Shader
	{
		friend class D3D11Buffer;
	public:
		D3D11Shader(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements);
		~D3D11Shader();

		virtual void Stage() const override;
		virtual const uint32_t GetOffset() const override;

		virtual const BufferLayout& GetLayout() const override;
	private:
		void CreateShaders(const ShaderGroup& shaderGroup, ID3D11Device* device);
		void CreateInputLayout(const ShaderGroup& shaderGroup, ID3D11Device* device);

		void RegisterShaderBuilders();
		void RegisterShaderStagers();

		void BuildVertexShader(std::string_view blobPath, ID3D11Device* device);
		void BuildPixelShader(std::string_view blobPath, ID3D11Device* device);
		void BuildGeometryShader(std::string_view blobPath, ID3D11Device* device);
		void BuildHullShader(std::string_view blobPath, ID3D11Device* device);
		void BuildDomainShader(std::string_view blobPath, ID3D11Device* device);
		void BuildComputeShader(std::string_view blobPath, ID3D11Device* device);

		void StageVertexShader(ID3D11DeviceContext* deviceContext);
		void StagePixelShader(ID3D11DeviceContext* deviceContext);
		void StageGeometryShader(ID3D11DeviceContext* deviceContext);
		void StageHullShader(ID3D11DeviceContext* deviceContext);
		void StageDomainShader(ID3D11DeviceContext* deviceContext);
		void StageComputeShader(ID3D11DeviceContext* deviceContext);

		DXGI_FORMAT GetFormat(ShaderDataType type);

		ComPtr<ID3D11InputLayout> m_InputLayout;

		ComPtr<ID3D11VertexShader> m_VertexShader;
		ComPtr<ID3DBlob> m_VertexBlob;
		ComPtr<ID3D11PixelShader> m_PixelShader;
		ComPtr<ID3DBlob> m_PixelBlob;
		ComPtr<ID3D11GeometryShader> m_GeometryShader;
		ComPtr<ID3DBlob> m_GeometryBlob;
		ComPtr<ID3D11HullShader> m_HullShader;
		ComPtr<ID3DBlob> m_HullBlob;
		ComPtr<ID3D11DomainShader> m_DomainShader;
		ComPtr<ID3DBlob> m_DomainBlob;
		ComPtr<ID3D11ComputeShader> m_ComputeShader;
		ComPtr<ID3DBlob> m_ComputeBlob;

		std::unordered_map<GRAPHICS_SHADER_KIND, std::function<void(std::string_view, ID3D11Device*)>> m_ShaderBuilders;
		mutable std::unordered_map<GRAPHICS_SHADER_KIND, std::function<void(ID3D11DeviceContext*)>> m_ShaderStagers;

		ID3D11DeviceContext* m_Context;

		BufferLayout m_Layout;
		ShaderGroup m_ShaderGroup;
	};
}

#endif //D3D11_SHADER_HPP

#endif
