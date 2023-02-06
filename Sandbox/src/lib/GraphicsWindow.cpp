#include "GraphicsWindow.hpp"

GraphicsWindow::GraphicsWindow(wxFrame* parent) :
	wxFrame(parent, wxID_ANY, "Graphics Context", wxDefaultPosition, wxSize(960,540))
{
	Show(true);
	auto size = this->GetClientSize();
	Connect(wxEVT_PAINT, wxPaintEventHandler(GraphicsWindow::OnPaint));
	Connect(wxEVT_DESTROY, wxWindowDestroyEventHandler(GraphicsWindow::OnDestroy));
	m_Application.reset(new Satoshi::Application((HWND)GetHandle(), size.x, size.y));
}

GraphicsWindow::~GraphicsWindow()
{
	m_Application.reset();
	Destroy();
}

void GraphicsWindow::Notify()
{
	if(!m_Destroyed)
		m_Application->Update();
}

void GraphicsWindow::OnPaint(wxPaintEvent& e)
{
	e.Skip();
}

void GraphicsWindow::OnDestroy(wxWindowDestroyEvent& e)
{
	m_Destroyed = true;
	m_Application.reset();
	e.Skip();
}

