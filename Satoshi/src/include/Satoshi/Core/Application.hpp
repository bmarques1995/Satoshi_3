#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Core.hpp"
#include "LayerStack.hpp"
#include "Satoshi/Window/Window.hpp"
#include <stpch.hpp>

#include "Satoshi/Events/ApplicationEvent.hpp"
#include "Satoshi/Events/KeyEvent.hpp"
#include "Satoshi/Events/MouseEvent.hpp"

#include "Satoshi/Renderer/GraphicsContext.hpp"
#include "Satoshi/Renderer/RendererAPI.hpp"
#include "Satoshi/Renderer/ShaderManager.hpp"

namespace Satoshi
{
    class SATOSHI_API Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);

        Window* GetWindow() { return m_Window.get(); }
        GraphicsContext* GetContext() { return m_Context.get(); }
        ShaderManager* GetShaderManager() { return m_ShaderManager.get(); }

        inline static Application* GetInstance() { return s_Instance; }

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

    private:
        static Application* s_Instance;
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

        void GetGraphicsBackend();
        void CreateWindowController();
        void CreateContext();
        void SetAPIAndGraphicsCardToTitle();

        std::unique_ptr<Window> m_Window;
        std::unique_ptr<GraphicsContext> m_Context;
        std::unique_ptr<ShaderManager> m_ShaderManager;
        LayerStack m_LayerStack;

        GRAPHICS_API m_API;
    };
}


#endif //APPLICATION_HPP
