#ifndef GRAPHICS_WINDOW_HPP
#define GRAPHICS_WINDOW_HPP

#include "Satoshi.hpp"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class GraphicsWindow : public wxFrame
{
public:
    GraphicsWindow(wxFrame* parent = nullptr);
    ~GraphicsWindow();

    void MyUpdate();
private:
    Satoshi::Application* m_Application;
};


#endif //GRAPHICS_WINDOW_HPP
