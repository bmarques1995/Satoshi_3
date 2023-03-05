#ifdef ST_PLATFORM_WINDOWS

#ifndef D3D12_BUFFER_HPP
#define D3D12_BUFFER_HPP

#include "Satoshi/Renderer/Buffer.hpp"
#include "D3D12Context.hpp"

namespace Satoshi
{
	class D3D12Buffer
	{
	protected:
		void CreateBuffer(const void* data, size_t size, ID3D12Device* device, D3D12MA::Allocator* allocator);

		ComPtr<ID3D12Resource> m_Buffer;
		ComPtr<D3D12MA::Allocation> m_Allocation;
	};

	class D3D12VertexBuffer : public VertexBuffer, public D3D12Buffer
	{
	public:
		D3D12VertexBuffer(const void* data, size_t size, uint32_t stride);
		~D3D12VertexBuffer();

		virtual void Stage() const override;
		virtual void RegisterLayout(IndexBuffer* indexBuffer) override;
	private:
		uint32_t m_Stride;
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	};

	class D3D12IndexBuffer : public IndexBuffer, public D3D12Buffer
	{
	public:
		D3D12IndexBuffer(const void* data, size_t count);
		~D3D12IndexBuffer();

		virtual void Stage() const override;
		virtual uint32_t GetCount() const override;
	private:
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	};

}

#endif //D3D12_BUFFER_HPP

#endif // ST_PLATFORM_WINDOWS
