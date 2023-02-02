#include "MainApplication.hpp"

bool MainApplication::OnInit()
{
    Satoshi::KeyPressedEvent e(65);
    Satoshi::Console::Init();
    Satoshi::Console::Log("Hello Logger");
    Satoshi::Console::Log(e.ToString());
    Satoshi::Console::End();
    m_Window = new MainWindow();
    m_Window->Show(true);
    //Base para GL4, D3D11, VK e D3D12 
    //auto hwnd = (HWND)frame->GetHandle();
    return true;
}

void MainApplication::Exit()
{
    delete m_Window;
}
