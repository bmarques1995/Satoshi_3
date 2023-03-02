#ifndef IMPL_LAYER_HPP
#define IMPL_LAYER_HPP

#include <stpch.hpp>
#include "Layer.hpp"
#include "Satoshi/Renderer/Shader.hpp"
#include "Satoshi/Renderer/Buffer.hpp"

namespace Satoshi
{
	

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
		//void CreateVertexBuffer(const void* data, size_t size, uint32_t stride, ID3D11Device* device);
		//void CreateIndexBuffer(const void* data, size_t size, ID3D11Device* device);

		
#pragma region Shader

		//ID3D11DeviceContext* m_Context;
		std::shared_ptr<Shader> m_Shader;

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

		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
#pragma endregion
	};
}

#endif //IMPL_LAYER_HPP
