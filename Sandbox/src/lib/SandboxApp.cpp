#include "Satoshi.hpp"

#include "MainApplication.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, 
    HINSTANCE hPrevInstance, 
    LPWSTR lpCmdLine,
    int nCmdShow)                             
{

    Satoshi::Application* app = new Satoshi::Application();
    app->Run();
    delete app;

#if 0 //Sample code for detached game
    std::wstring cmdLine = lpCmdLine;
    std::string s_cmdLine = std::string(cmdLine.begin(), cmdLine.end());
    return wxEntry(hInstance, hPrevInstance, (char *)s_cmdLine.c_str(), nCmdShow);
#endif
}

//#define wxIMPLEMENT_APP_NO_MAIN(appname)                                    
MainApplication& wxGetApp()
{ 
    return *static_cast<MainApplication*>(wxApp::GetInstance());
}
    
wxAppConsole *wxCreateApp()                                             
{                                                                       
        wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "your program");                    
        return new MainApplication;
}                                                                       

wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction)wxCreateApp);