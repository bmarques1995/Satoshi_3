#ifndef MAIN_APPLICATION_HPP
#define MAIN_APPLICATION_HPP

#include "MainWindow.hpp"
#include "GraphicsWindow.hpp"
#include "Satoshi.hpp"
#include "Satoshi/Events/KeyEvent.hpp"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class MainApplication : public wxApp
{
public:
    virtual bool OnInit() override;
    virtual void Exit() override;

    virtual int OnRun() override;
private:
    MainWindow* m_Window;
    GraphicsWindow* m_GraphicsWindow;
    
};


#endif //MAIN_APPLICATION_HPP
