#include <Satoshi.hpp>

int WINAPI wWinMain(HINSTANCE hInstance, 
    HINSTANCE hPrevInstance, 
    LPWSTR lpCmdLine,
    int nCmdShow)                             
{

    Satoshi::Application* app = new Satoshi::Application();
    app->Run();
    delete app;
}
