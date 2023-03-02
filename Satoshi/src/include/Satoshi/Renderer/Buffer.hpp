#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <stpch.hpp>
#include "Satoshi/Renderer/RendererAPI.hpp"
#include "Satoshi/Renderer/Shader.hpp"

namespace Satoshi
{
	class VertexBuffer
	{
	public:
		virtual void Stage() const = 0;
		virtual void RegisterLayout() = 0;

		static VertexBuffer* Create(const void* data, size_t size, const BufferLayout& layout, GRAPHICS_API api);
	};

	class IndexBuffer
	{
	public:
		virtual void Stage() const = 0;
		virtual uint32_t GetCount() const = 0;

		static IndexBuffer* Create(const void* data, size_t count, GRAPHICS_API api);
	protected:
		uint32_t m_Count;
	};
}

#endif //BUFFER_HPP
