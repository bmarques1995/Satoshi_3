#ifndef LAYER_HPP
#define LAYER_HPP

#include <stpch.hpp>
#include "Core.hpp"
#include "Satoshi/Events/Event.hpp"

namespace Satoshi
{
    class SATOSHI_API Layer
    {
    public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;

    private:

    };
}


#endif //LAYER_HPP
