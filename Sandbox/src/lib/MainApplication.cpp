#include "MainApplication.hpp"

bool MainApplication::OnInit()
{
    Satoshi::KeyPressedEvent e(65);
    Satoshi::Console::Init();
    Satoshi::Console::Log("Hello Logger");
    Satoshi::Console::Log(e.ToString());
    Satoshi::Console::End();
    m_Window = new MainWindow();
    m_GraphicsWindow = new GraphicsWindow(m_Window);

    //Base para GL4, D3D11, VK e D3D12 
    //auto hwnd = (HWND)frame->GetHandle();
    return true;
}

void MainApplication::Exit()
{
    delete m_GraphicsWindow;
    delete m_Window;
}

int MainApplication::OnRun()
{
    m_GraphicsWindow->MyUpdate();
    return wxApp::OnRun();
}
