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

    void Notify();

    void OnPaint(wxPaintEvent& e);
    void OnDestroy(wxWindowDestroyEvent& e);
private:
    std::unique_ptr<Satoshi::InEngineApplication> m_Application;
    bool m_Destroyed = false;
};


#endif //GRAPHICS_WINDOW_HPP
