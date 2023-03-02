#include "Satoshi/Core/ImplLayer.hpp"

#include "Satoshi.hpp"


Satoshi::ImplLayer::ImplLayer(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements)
{
    auto currentAPI = Application::GetInstance()->GetCurrentAPI();
    m_Shader.reset(Shader::Create(shaderGroup, elements, currentAPI));
    m_VertexBuffer.reset(VertexBuffer::Create(vBuffer, sizeof(vBuffer), m_Shader->GetLayout(), currentAPI));
    m_IndexBuffer.reset(IndexBuffer::Create(iBuffer, sizeof(iBuffer)/sizeof(uint32_t), currentAPI));
    
    m_VertexBuffer->Stage();
    m_IndexBuffer->Stage();
    m_VertexBuffer->RegisterLayout();
}

void Satoshi::ImplLayer::OnAttach()
{
}

void Satoshi::ImplLayer::OnDetach()
{
}

void Satoshi::ImplLayer::OnUpdate()
{
	if (Input::IsKeyPressed(ST_KEY_C))
		Console::Log("Key C was pressed");
    m_VertexBuffer->Stage();
    m_IndexBuffer->Stage();
    m_Shader->Stage();
    
}

void Satoshi::ImplLayer::OnEvent(Event& event)
{
}