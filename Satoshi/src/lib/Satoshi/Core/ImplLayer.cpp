#include "Satoshi/Core/ImplLayer.hpp"

#include "Satoshi.hpp"

Satoshi::ImplLayer::ImplLayer()
{
}

void Satoshi::ImplLayer::OnAttach()
{
}

void Satoshi::ImplLayer::OnDetach()
{
}

void Satoshi::ImplLayer::OnUpdate()
{
	if (Input::IsKeyPressed(ST_KEY_C))
		Console::Log("Key C was pressed");
}

void Satoshi::ImplLayer::OnEvent(Event& event)
{
}
