#include "RenderTimer.hpp"

RenderTimer::RenderTimer(GraphicsWindow* frame) : wxTimer(frame)
{
    m_Frame = frame;
}

void RenderTimer::Notify()
{
    m_Frame->Notify();
}

void RenderTimer::MyStart()
{
    wxTimer::Start(5);
}