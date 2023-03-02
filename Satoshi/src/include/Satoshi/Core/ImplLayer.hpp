#ifndef IMPL_LAYER_HPP
#define IMPL_LAYER_HPP

#include <stpch.hpp>
#include "Layer.hpp"
#include "Platform/Graphics/D3D11/D3D11Context.hpp"

namespace Satoshi
{
	enum class ShaderDataType
	{
		None = 0,
		Float,
		Float2,
		Float3,
		Float4,
		Uint,
		Uint2,
		Uint3,
		Uint4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float: return 4;
		case ShaderDataType::Float2: return 4 * 2;
		case ShaderDataType::Float3: return 4 * 3;
		case ShaderDataType::Float4: return 4 * 4;
		case ShaderDataType::Uint: return 4;
		case ShaderDataType::Uint2: return 4 * 2;
		case ShaderDataType::Uint3: return 4 * 3;
		case ShaderDataType::Uint4: return 4 * 4;
		case ShaderDataType::Bool: return 1;
		default: assert(false); return 0;
		}
	}

	struct BufferElement
	{
		std::string Name;
		uint32_t Offset;
		uint32_t Size;
		ShaderDataType Type;
		bool Normalized;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{}

		uint32_t GetElementCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Bool:
			case ShaderDataType::Float:
			case ShaderDataType::Uint:
				return 1Ui32;
			case ShaderDataType::Float2:
			case ShaderDataType::Uint2:
				return 2Ui32;
			case ShaderDataType::Float3:
			case ShaderDataType::Uint3:
				return 3Ui32;
			case ShaderDataType::Float4:
			case ShaderDataType::Uint4:
				return 4Ui32;
			default:
				return 0Ui32;
			}
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetAndStride();
		}

		inline const uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

	private:

		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class ImplLayer : public Layer
	{
	public:
		ImplLayer(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements);
		~ImplLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Event& event) override;
	private:
		void CreateShaders(const ShaderGroup& shaderGroup, ID3D11Device* device);
		void CreateInputLayout(const ShaderGroup& shaderGroup, ID3D11Device* device);
		void CreateVertexBuffer(const void* data, size_t size, uint32_t stride, ID3D11Device* device);
		void CreateIndexBuffer(const void* data, size_t size, ID3D11Device* device);

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

#pragma region Shader

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
		
		std::unordered_map<SHADER_KIND, std::function<void(std::string_view, ID3D11Device*)>> m_ShaderBuilders;
		std::unordered_map<SHADER_KIND, std::function<void(ID3D11DeviceContext*)>> m_ShaderStagers;

		ID3D11DeviceContext* m_Context;

		BufferLayout m_Layout;
		ShaderGroup m_ShaderGroup;

#pragma endregion

#pragma region Buffer

		float vBuffer[21] =
		{
			.0f, .5f, .0f, 1.0f, .0f, .0f, 1.0f,
			.5f, -.5f, .0f, .0f, 1.0f, .0f, 1.0f,
			-.5f, -.5f, .0f, .0f, .0f, 1.0f, 1.0f
		};

		uint32_t iBuffer[3] =
		{
			0,1,2
		};

		ComPtr<ID3D11Buffer> m_VertexBuffer;
		ComPtr<ID3D11Buffer> m_IndexBuffer;

		uint32_t m_VBStride;
		uint32_t m_IBCount;
#pragma endregion
	};
}

#endif //IMPL_LAYER_HPP
