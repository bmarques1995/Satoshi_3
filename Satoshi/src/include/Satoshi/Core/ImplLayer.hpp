#ifndef IMPL_LAYER_HPP
#define IMPL_LAYER_HPP

#include <stpch.hpp>
#include "Layer.hpp"
#include "Satoshi/Renderer/Shader.hpp"
#include "Satoshi/Renderer/Buffer.hpp"

#include "Platform/Graphics/D3D12/D3D12Context.hpp"

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

		std::shared_ptr<Shader> m_Shader;

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

		uint32_t m_IndexCount;

		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;

	};
}

#endif //IMPL_LAYER_HPP
