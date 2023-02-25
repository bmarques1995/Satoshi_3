#include "Satoshi/Core/Application.hpp"
#include "Satoshi/Core/Console.hpp"
#include "Satoshi/Events/ApplicationEvent.hpp"
#include "Satoshi/Core/ApplicationStarter.hpp"
#include "Satoshi/Window/Input.hpp"
#include "Satoshi/Window/InputCodes.hpp"

Satoshi::Application* Satoshi::Application::s_Instance = nullptr;

Satoshi::Application::Application()
{
	s_Instance = this;
	Console::Init();
	

	ApplicationStarter::BuildStarter();
	json startupJson = ApplicationStarter::GetStartupJson();
	auto test = startupJson["GraphicsAPI"].get<std::string>();
	m_API = RendererAPI::MatchAPIByName(test);
	
	m_Window.reset(Window::Create());
	m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	Satoshi::Input::Start(std::any_cast<StWindowHandle>(m_Window->GetNativeWindow()));
	
	m_Context.reset(GraphicsContext::Create(std::any_cast<StWindowHandle>(m_Window->GetNativeWindow()), m_Window->GetWidth(), m_Window->GetHeight(), m_API));
	m_Context->SetVSync(true);
	std::string gpuName;
	m_Context->GetGPUName(&gpuName);
	
	m_ShaderManager.reset(ShaderManager::Create(m_API));
	Satoshi::ShaderGroup shaderGroup("Triangle", m_API,
		{
			{Satoshi::SHADER_KIND::SHADER_KIND_VERTEX},
			{Satoshi::SHADER_KIND::SHADER_KIND_PIXEL}
		}
	);
	m_ShaderManager->CompileShaderFamily(shaderGroup);
}

Satoshi::Application::~Application()
{
	Satoshi::Input::End();
	m_Context.reset();
	m_Window.reset();
	Console::End();
}

void Satoshi::Application::Run()
{
	while (!m_Window->ShouldClose())
	{
		m_Window->OnUpdate();
		m_Context->NewFrame();
		m_Context->ReceiveCommands();
		m_Context->ClearTarget();
		m_Context->DispatchCommands();
		m_Context->Present();
		m_Context->EndFrame();
		if (Satoshi::Input::IsKeyPressed(ST_KEY_A))
			Satoshi::Console::Log("Key A was pressed");
	}
}

void Satoshi::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
	dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));
	Console::Log(e.ToString());
}

bool Satoshi::Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Window->SetCloseState(true);
	return true;
}

bool Satoshi::Application::OnWindowResize(WindowResizeEvent& e)
{
	m_Context->OnResize(e);
	return false;
}

