#ifndef IMPL_LAYER_HPP
#define IMPL_LAYER_HPP

#include <stpch.hpp>
#include "Layer.hpp"

namespace Satoshi
{
	class ImplLayer : public Layer
	{
	public:
		ImplLayer();
		~ImplLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Event& event) override;
	};
}

#endif //IMPL_LAYER_HPP
