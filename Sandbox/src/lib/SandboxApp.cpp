#include <Satoshi.hpp>
#include "DevLayer.hpp"


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

