#ifndef IMPL_LAYER_HPP
#define IMPL_LAYER_HPP

#include <stpch.hpp>
#include "Layer.hpp"
#include "Satoshi/Renderer/Shader.hpp"
#include "Satoshi/Renderer/Buffer.hpp"
#include "Satoshi/Renderer/GraphicsContext.hpp"

#include <Eigen/Dense>

namespace Satoshi
{

#ifdef ST_PLATFORM_WINDOWS

#pragma pack(push, 1)
	//alignment padding, se for criar um vertex input, aglutinar tudo em um Eigen::Vector só
	struct VertexData
	{
		Eigen::Vector<float, 7> PosCol;
	};
#pragma pack(pop)

#endif
	class ImplLayer : public Layer
	{
	public:
		ImplLayer(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements, GraphicsContext* context);
		~ImplLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Event& event) override;
	private:

		std::shared_ptr<Shader> m_Shader;

		VertexData vBuffer[4] =
		{
			{ Eigen::Vector<float, 7>(-.5f, -.5f, .0f, 1.0f, .0f, .0f, 1.0f) },
			{ Eigen::Vector<float, 7>(-.5f, .5f, .0f, .0f, 1.0f, .0f, 1.0f) },
			{ Eigen::Vector<float, 7>(.5f, -.5f, .0f, .0f, .0f, 1.0f, 1.0f) },
			{ Eigen::Vector<float, 7>(.5f, .5f, .0f, 1.0f, 1.0f, .0f, 1.0f) }
		};
		
		uint32_t iBuffer[6] =
		{
			3,2,1,
			1,2,0
		};

		uint32_t m_IndexCount;

		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;

		GraphicsContext* m_Context;
	};
}

#endif //IMPL_LAYER_HPP
