#ifdef ST_PLATFORM_WINDOWS

#ifndef WIN32_INPUT_HPP
#define WIN32_INPUT_HPP

#include "Satoshi/Window/Input.hpp"

namespace Satoshi
{
    class Win32Input : public Input
    {
    public:
        Win32Input(StWindowHandle window);
        ~Win32Input();

    protected:
        virtual bool IsKeyPressedImpl(uint32_t keycode) override;
        virtual bool IsMouseButtonPressedImpl(uint32_t button) override;
        virtual float GetMouseXImpl() override;
        virtual float GetMouseYImpl() override;
        virtual std::pair<float, float> GetMousePosImpl() override;
    private:
        StWindowHandle m_Window;
    };
}


#endif //WIN32_INPUT_HPP

#endif // ST_PLATFORM_WINDOWS
