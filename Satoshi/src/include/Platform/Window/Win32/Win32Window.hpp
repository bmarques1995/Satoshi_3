#ifdef ST_PLATFORM_WINDOWS

#ifndef WIN32_WINDOW_HPP
#define WIN32_WINDOW_HPP

#include "Satoshi/Window/Window.hpp"

#include <stpch.hpp>
#include <windows.h>

namespace Satoshi
{
    struct WindowData
    {
        std::wstring Title;
        bool VSync;
        uint32_t Width, Height;
        EventCallbackFn EventCallback;
        std::unordered_map<UINT, std::function<void(HWND,WPARAM,LPARAM,WindowData*)>> CallbackResolver;
    };
    class Win32Window : public Window
    {
    public:
        Win32Window(const WindowProps& props);
        ~Win32Window();

        virtual void OnUpdate() override;

        virtual uint32_t GetWidth() const override;
        virtual uint32_t GetHeight() const override;

        virtual bool ShouldClose() const override;
        virtual void SetCloseState(bool value) override;

        virtual void SetEventCallback(const EventCallbackFn& callback) override;

        virtual std::any GetNativeWindow() const override;

    private:

        void CreateWindowClass(HINSTANCE* instance);
        static void AdjustDimensions(LPRECT originalDimensions, DWORD flags);
        void RegisterCallbacks();

        

        HWND m_WindowHandle;
        WNDCLASSEXW m_WindowClass = { 0 };
        MSG m_MSG = { 0 };

        WindowData m_WindowData;
        bool m_ShouldClose;


    };
}


#endif //WIN32_WINDOW_HPP


#endif // ST_PLATFORM_WINDOWS
