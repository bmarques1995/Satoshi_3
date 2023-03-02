#include "Satoshi/Core/Application.hpp"
#include "Satoshi/Core/Console.hpp"
#include "Satoshi/Core/ImplLayer.hpp"
#include "Satoshi/Events/ApplicationEvent.hpp"
#include "Satoshi/Core/ApplicationStarter.hpp"
#include "Satoshi/Window/Input.hpp"
#include "Satoshi/Window/InputCodes.hpp"

Satoshi::Application* Satoshi::Application::s_Instance = nullptr;

Satoshi::Application::Application()
{
	s_Instance = this;
	Console::Init();
	
	GetGraphicsBackend();
	CreateWindowController();
	CreateContext();
	SetAPIAndGraphicsCardToTitle();
	
	m_ShaderManager.reset(ShaderManager::Create(m_API));
	
	Satoshi::ShaderGroup shaderGroup("Triangle", m_API,
		{
			{Satoshi::SHADER_KIND::SHADER_KIND_VERTEX},
			{Satoshi::SHADER_KIND::SHADER_KIND_PIXEL}
		}
	);
	m_ShaderManager->CompileShaderFamily(shaderGroup);

	std::initializer_list<Satoshi::BufferElement> bufferElements = {
			{Satoshi::ShaderDataType::Float3, "POSITION", false},
			{Satoshi::ShaderDataType::Float4, "COLOR", false}
	};


#if defined DEBUG || defined _DEBUG
	if(m_API == GRAPHICS_API::D3D11)
		PushLayer(new ImplLayer(shaderGroup, bufferElements));
#endif
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
		for (Layer* layer : m_LayerStack)
			layer->OnUpdate();
		m_Context->Draw(3);
		m_Context->DispatchCommands();
		m_Context->Present();
		m_Context->EndFrame();
		
	}
}

void Satoshi::Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
	dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));
	Console::Log(e.ToString());
}

void Satoshi::Application::PushLayer(Layer* layer)
{
	m_LayerStack.PushLayer(layer);
}

void Satoshi::Application::PushOverlay(Layer* overlay)
{
	m_LayerStack.PushOverlay(overlay);
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

void Satoshi::Application::GetGraphicsBackend()
{
	ApplicationStarter::BuildStarter();
	Json::Value startupJson = ApplicationStarter::GetStartupJson();
	auto graphicsAPI = startupJson["GraphicsAPI"].as<std::string>();
	std::transform(graphicsAPI.begin(), graphicsAPI.end(), graphicsAPI.begin(), ::toupper);
	m_API = RendererAPI::MatchAPIByName(graphicsAPI);
}

void Satoshi::Application::CreateWindowController()
{
	m_Window.reset(Window::Create());
	m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	Satoshi::Input::Start(std::any_cast<StWindowHandle>(m_Window->GetNativeWindow()));
}

void Satoshi::Application::CreateContext()
{
	m_Context.reset(GraphicsContext::Create(std::any_cast<StWindowHandle>(m_Window->GetNativeWindow()), m_Window->GetWidth(), m_Window->GetHeight(), m_API));
	m_Context->SetVSync(true);
	m_Context->SetClearColor(.3f, .6f, .9f, 1.0f);
}

void Satoshi::Application::SetAPIAndGraphicsCardToTitle()
{
	std::stringstream windowName;

	Json::Value startupJson = ApplicationStarter::GetStartupJson();
	auto graphicsAPI = startupJson["GraphicsAPI"].as<std::string>();
	std::transform(graphicsAPI.begin(), graphicsAPI.end(), graphicsAPI.begin(), ::toupper);

	std::string gpuName;
	m_Context->GetGPUName(&gpuName);
	Console::Log(gpuName);

	windowName << m_Window->GetTitle() << " [" << graphicsAPI << "] " << gpuName;
	m_Window->SetTitle(windowName.str());
}

