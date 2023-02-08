#include "Satoshi/Core/InEngineApplication.hpp"
#include "Satoshi/Core/Console.hpp"
#include "Satoshi/Core/ApplicationStarter.hpp"
#include "Satoshi/Window/Input.hpp"
#include "Satoshi/Window/InputCodes.hpp"

Satoshi::InEngineApplication* Satoshi::InEngineApplication::s_Instance;

Satoshi::InEngineApplication::InEngineApplication(StWindowHandle windowHandle, uint32_t width, uint32_t height) :
	m_WindowHandle(windowHandle), m_Width(width), m_Height(height)
{
	s_Instance = this;
	Satoshi::Console::Init();
	Satoshi::Input::Start(m_WindowHandle);
	
	ApplicationStarter::BuildStarter();
	json startupJson = ApplicationStarter::GetStartupJson();
	auto test = startupJson["GraphicsAPI"].get<std::string>();
	m_API = RendererAPI::MatchAPIByName(test);
	
	
	m_Context.reset(GraphicsContext::Create(windowHandle, width, height, m_API));
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

Satoshi::InEngineApplication::~InEngineApplication()
{
	Satoshi::Input::End();
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
	return m_Width;
}

uint32_t Satoshi::InEngineApplication::GetHeight()
{
	return m_Height;
}

void Satoshi::InEngineApplication::Resize(WindowResizeEvent& e)
{
	m_Width = e.GetWidth();
	m_Height = e.GetHeight();
	m_Context->OnResize(e);
}

void Satoshi::InEngineApplication::Update()
{
	m_Context->NewFrame();
	m_Context->ReceiveCommands();
	m_Context->ClearTarget();
	m_Context->DispatchCommands();
	m_Context->Present();
	m_Context->EndFrame();
	if (Satoshi::Input::IsKeyPressed(ST_KEY_A))
		Satoshi::Console::Log("Key A was pressed");
}