#ifndef DEV_LAYER_HPP
#define DEV_LAYER_HPP

#include <Satoshi.hpp>

class DevLayer : public Satoshi::Layer
{
public:
	DevLayer();
	virtual ~DevLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate() override;
	virtual void OnEvent(Satoshi::Event& event) override;
};

#endif //DEV_LAYER_HPP
