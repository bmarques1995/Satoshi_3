#include "DevLayer.hpp"

DevLayer::DevLayer() :
	Satoshi::Layer("Development Layer")
{
}

void DevLayer::OnAttach()
{
}

void DevLayer::OnDetach()
{
}

void DevLayer::OnUpdate()
{
	if (Satoshi::Input::IsKeyPressed(ST_KEY_A))
		Satoshi::Console::Log("Key A was pressed");
}

void DevLayer::OnEvent(Satoshi::Event& event)
{
}