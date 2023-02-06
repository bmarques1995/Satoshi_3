#include "MainApplication.hpp"

bool MainApplication::OnInit()
{
    
    m_Window = new MainWindow();
    m_GraphicsWindow = new GraphicsWindow(m_Window);
    m_RenderTimer = new RenderTimer(m_GraphicsWindow);
    m_RenderTimer->MyStart();

    //Base para GL4, D3D11, VK e D3D12 
    //auto hwnd = (HWND)frame->GetHandle();
    return true;
}

void MainApplication::Exit()
{
    delete m_RenderTimer;
    delete m_GraphicsWindow;
    delete m_Window;
}

int MainApplication::OnRun()
{
    return wxApp::OnRun();
}
