#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <stpch.hpp>

#include "Satoshi/Core/Core.hpp"
#include "Satoshi/Events/Event.hpp"

namespace Satoshi
{
    using EventCallbackFn = std::function<void(Event&)>;
    struct WindowProps
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;

        WindowProps
        (
            const std::string& title = "Satoshi Engine",
            uint32_t width = 1280,
            uint32_t height = 720
        ) : Title(title), Width(width), Height(height)
        {

        }
    };

    class SATOSHI_API Window
    {
    public:
        virtual ~Window() {}

        virtual void OnUpdate() = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual std::any GetNativeWindow() const = 0;

        virtual bool ShouldClose() const = 0;
        virtual void SetCloseState(bool value) = 0;

        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        virtual std::string GetTitle() = 0;
        virtual void SetTitle(const std::string& Title) = 0;
        
        static Window* Create(const WindowProps& props = WindowProps());
    private:

    };
}


#endif //WINDOW_HPP
