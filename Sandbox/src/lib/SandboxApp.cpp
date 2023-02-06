#include "Satoshi.hpp"

#include "MainApplication.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, 
    HINSTANCE hPrevInstance, 
    LPWSTR lpCmdLine,
    int nCmdShow)                             
{
    std::wstring cmdLine = lpCmdLine;
    std::string s_cmdLine = std::string(cmdLine.begin(), cmdLine.end());
    return wxEntry(hInstance, hPrevInstance, (char *)s_cmdLine.c_str(), nCmdShow);
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