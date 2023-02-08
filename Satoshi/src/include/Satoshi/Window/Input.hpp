#ifndef INPUT_HPP
#define INPUT_HPP

#include "Satoshi/Core/Core.hpp"
#include <stpch.hpp>

namespace Satoshi
{
    class SATOSHI_API Input
    {
    public:
        static bool IsKeyPressed(uint32_t keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
        static bool IsMouseButtonPressed(uint32_t button) { return s_Instance->IsMouseButtonPressedImpl(button); }
        static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
        static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
        static std::pair<float, float> GetMousePos() { return s_Instance->GetMousePosImpl(); }
        
        static void Start(StWindowHandle window);
        static void End();
    protected:
        virtual bool IsKeyPressedImpl(uint32_t keycode) = 0;
        virtual bool IsMouseButtonPressedImpl(uint32_t button) = 0;
        virtual float GetMouseXImpl() = 0;
        virtual float GetMouseYImpl() = 0;
        virtual std::pair<float, float> GetMousePosImpl() = 0;

    private:
        static Input* s_Instance;
    };
}


#endif //INPUT_HPP