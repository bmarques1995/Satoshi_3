#include "Satoshi/Core/InEngineApplication.hpp"
#include "Satoshi/Core/Console.hpp"
#include "Satoshi/Core/ApplicationStarter.hpp"

Satoshi::InEngineApplication* Satoshi::InEngineApplication::s_Instance;

Satoshi::InEngineApplication::InEngineApplication(StWindowHandle windowHandle, uint32_t width, uint32_t height)
{
	s_Instance = this;
	ApplicationStarter::BuildStarter();
	json startupJson = ApplicationStarter::GetStartupJson();
	auto test = startupJson["GraphicsAPI"].get<std::string>();
	m_API = RendererAPI::MatchAPIByName(test);
	Satoshi::Console::Init();
	m_WindowHandle = windowHandle;
	m_Width = width;
	m_Height = height;
	m_Context.reset(GraphicsContext::Create(windowHandle, width, height, m_API));
	m_Context->SetVSync(true);
	std::string gpuName;
	m_Context->GetGPUName(&gpuName);
	Satoshi::Console::Log(gpuName);
}

Satoshi::InEngineApplication::~InEngineApplication()
{
	Satoshi::Console::End();
}

StWindowHandle Satoshi::InEngineApplication::GetNativeWindow()
{
	return m_WindowHandle;
}

Satoshi::InEngineApplication* Satoshi::InEngineApplication::GetInstance()
{
	return s_Instance;
}

uint32_t Satoshi::InEngineApplication::GetWidth()
{
	return 0;
}

uint32_t Satoshi::InEngineApplication::GetHeight()
{
	return 0;
}

void Satoshi::InEngineApplication::Update()
{
	m_Context->NewFrame();
	m_Context->ReceiveCommands();
	m_Context->ClearTarget();
	m_Context->DispatchCommands();
	m_Context->Present();
	m_Context->EndFrame();
}