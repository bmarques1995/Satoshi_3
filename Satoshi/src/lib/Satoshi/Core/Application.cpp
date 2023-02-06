#include "Satoshi/Core/Application.hpp"

Satoshi::Application* Satoshi::Application::s_Instance;

Satoshi::Application::Application(StWindowHandle windowHandle, uint32_t width, uint32_t height)
{
	s_Instance = this;
	m_WindowHandle = windowHandle;
	m_Width = width;
	m_Height = height;
	m_Context.reset(GraphicsContext::Create(GRAPHICS_API::D3D11));
}

Satoshi::Application::~Application()
{
}

StWindowHandle Satoshi::Application::GetNativeWindow()
{
	return m_WindowHandle;
}

Satoshi::Application* Satoshi::Application::GetInstance()
{
	return s_Instance;
}

uint32_t Satoshi::Application::GetWidth()
{
	return 0;
}

uint32_t Satoshi::Application::GetHeight()
{
	return 0;
}

void Satoshi::Application::Update()
{
	m_Context->NewFrame();
	m_Context->ReceiveCommands();
	m_Context->ClearTarget();
	m_Context->DispatchCommands();
	m_Context->Present();
	m_Context->EndFrame();
}
