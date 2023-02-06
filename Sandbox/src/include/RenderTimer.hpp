#ifndef RENDER_TIMER_HPP
#define RENDER_TIMER_HPP

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "GraphicsWindow.hpp"

class RenderTimer : public wxTimer
{
    GraphicsWindow* m_Frame;
public:
    RenderTimer(GraphicsWindow* frame);
    void Notify();
    void MyStart();
};


#endif //RENDER_TIMER_HPP
