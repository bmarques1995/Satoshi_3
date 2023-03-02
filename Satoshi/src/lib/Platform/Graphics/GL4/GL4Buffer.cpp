#include "Platform/Graphics/GL4/GL4Buffer.hpp"
#include "Satoshi.hpp"

Satoshi::GL4VertexBuffer::GL4VertexBuffer(const void* data, size_t size, const BufferLayout& layout) :
	m_Layout(layout)
{
	glGenVertexArrays(1, &m_VertexArray);
	glGenBuffers(1, &m_VertexBuffer);
	glBindVertexArray(m_VertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

Satoshi::GL4VertexBuffer::~GL4VertexBuffer()
{
	glDeleteBuffers(1, &m_VertexBuffer);
	glDeleteVertexArrays(1, &m_VertexArray);
}

void Satoshi::GL4VertexBuffer::Stage() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBindVertexArray(m_VertexArray);
}

void Satoshi::GL4VertexBuffer::RegisterLayout()
{
	auto nativeElements = m_Layout.GetElements();

	for (size_t i = 0; i < nativeElements.size(); i++)
	{
		glVertexAttribPointer(i, nativeElements[i].GetElementCount(), GL_FLOAT, GL_FALSE, m_Layout.GetStride(), (void*)nativeElements[i].Offset);
		glEnableVertexAttribArray(i);
	}
}

Satoshi::GL4IndexBuffer::GL4IndexBuffer(const void* data, size_t count)
{
	m_Count = (uint32_t)count;
	glGenBuffers(1, &m_IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
}

Satoshi::GL4IndexBuffer::~GL4IndexBuffer()
{
	glDeleteBuffers(1, &m_IndexBuffer);
}

void Satoshi::GL4IndexBuffer::Stage() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
}

uint32_t Satoshi::GL4IndexBuffer::GetCount() const
{
	return 0;
}
