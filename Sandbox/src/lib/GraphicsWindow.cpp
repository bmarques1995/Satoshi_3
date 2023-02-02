#include "GraphicsWindow.hpp"

GraphicsWindow::GraphicsWindow(wxFrame* parent) :
	wxFrame(parent, wxID_ANY, "Graphics Context", wxDefaultPosition, wxSize(960,540))
{
	Show(true);
	auto size = this->GetSize();
	m_Application = new Satoshi::Application((HWND)GetHandle(), size.x, size.y);
}

GraphicsWindow::~GraphicsWindow()
{
}

void GraphicsWindow::MyUpdate()
{
	m_Application->Update();
}

