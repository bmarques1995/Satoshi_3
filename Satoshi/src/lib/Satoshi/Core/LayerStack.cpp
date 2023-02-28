#include "Satoshi/Core/LayerStack.hpp"

Satoshi::LayerStack::LayerStack()
{
	m_LayerInsertIndex = m_Layers.size();
}

Satoshi::LayerStack::~LayerStack()
{
	for (Layer* layer : m_Layers)
		delete layer;
}

void Satoshi::LayerStack::PushLayer(Layer* layer)
{
	m_Layers.insert(advance(m_LayerInsertIndex), layer);
	layer->OnAttach();
	m_LayerInsertIndex++;
}

void Satoshi::LayerStack::PushOverlay(Layer* overlay)
{
	m_Layers.emplace_back(overlay);
	overlay->OnAttach();
	m_LayerInsertIndex++;
}

void Satoshi::LayerStack::PopLayer(Layer* layer)
{
	auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
	if (it != m_Layers.end())
	{
		layer->OnDetach();
		m_Layers.erase(it);
		m_LayerInsertIndex--;
	}
}

void Satoshi::LayerStack::PopOverlay(Layer* overlay)
{
	auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
	if (it != m_Layers.end())
	{
		overlay->OnDetach();
		m_Layers.erase(it);
		m_LayerInsertIndex--;
	}
}