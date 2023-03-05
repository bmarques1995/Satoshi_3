#ifndef GL4_BUFFER_HPP
#define GL4_BUFFER_HPP

#include "Satoshi/Renderer/Buffer.hpp"
#include "GL4Context.hpp"

namespace Satoshi
{
    class GL4VertexBuffer : public VertexBuffer
    {
    public:
        GL4VertexBuffer(const void* data, size_t size, const BufferLayout& layout);
        ~GL4VertexBuffer();

        virtual void Stage() const override;
        virtual void RegisterLayout(IndexBuffer* indexBuffer) override;

    private:
        uint32_t m_VertexBuffer, m_VertexArray;
        BufferLayout m_Layout;
    };

    class GL4IndexBuffer : public IndexBuffer
    {
    public:
        GL4IndexBuffer(const void* data, size_t count);
        ~GL4IndexBuffer();

        virtual void Stage() const override;
        virtual uint32_t GetCount() const override;
    private:
        uint32_t m_IndexBuffer;
    };
}


#endif //GL4_BUFFER_HPP
