#include <Satoshi.hpp>

class DevLayer : public Satoshi::Layer
{
public:
	DevLayer();
	virtual ~DevLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate() override;
	virtual void OnEvent(Satoshi::Event& event) override;
};

int WINAPI wWinMain(HINSTANCE hInstance, 
    HINSTANCE hPrevInstance, 
    LPWSTR lpCmdLine,
    int nCmdShow)                             
{

    Satoshi::Application* app = new Satoshi::Application();
	app->PushOverlay(new DevLayer());
    app->Run();
    delete app;
}

DevLayer::DevLayer() :
	Satoshi::Layer("Development Layer")
{
}

void DevLayer::OnAttach()
{
}

void DevLayer::OnDetach()
{
}

void DevLayer::OnUpdate()
{
	if (Satoshi::Input::IsKeyPressed(ST_KEY_A))
		Satoshi::Console::Log("Key A was pressed");
}

void DevLayer::OnEvent(Satoshi::Event& event)
{
}
