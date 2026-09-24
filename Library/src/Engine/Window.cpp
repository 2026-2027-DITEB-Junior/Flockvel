#include "pch.h"
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <cstdio>
#include "Window.h"

namespace clc
{
	bool Window::Initialize()
	{
        if (m_initialized)
            return true;

        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
        {
            printf("Error: SDL_Init(): %s\n", SDL_GetError());
            return false;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        if (m_Title.empty())
            m_Title = "Window";

        const SDL_WindowFlags windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        // SDL uses window coordinates here. HIGH_PIXEL_DENSITY only affects the
        // framebuffer size, so multiplying by the display scale would make the
        // logical window unexpectedly larger on HiDPI displays.
        m_window = SDL_CreateWindow(m_Title.c_str(), m_Width, m_Height, windowFlags);
        if (m_window == nullptr)
        {
            printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
            SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);
            return false;
        }

        m_glContext = SDL_GL_CreateContext(m_window);
        if (m_glContext == nullptr)
        {
            printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
            SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);
            return false;
        }

        if (!SDL_GL_MakeCurrent(m_window, m_glContext))
        {
            printf("Error: SDL_GL_MakeCurrent(): %s\n", SDL_GetError());
            SDL_GL_DestroyContext(m_glContext);
            SDL_DestroyWindow(m_window);
            m_glContext = nullptr;
            m_window = nullptr;
            SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);
            return false;
        }

        glewExperimental = GL_TRUE;
        GLenum glewErr = glewInit();
        if (glewErr != GLEW_OK)
        {
            printf("Error: glewInit(): %s\n", glewGetErrorString(glewErr));
            SDL_GL_DestroyContext(m_glContext);
            SDL_DestroyWindow(m_window);
            m_glContext = nullptr;
            m_window = nullptr;
            SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);
            return false;
        }

        SDL_GL_SetSwapInterval(1); // VSync; unsupported drivers may ignore it.
        SDL_SetWindowPosition(m_window, m_PosX, m_PosY);
        SDL_ShowWindow(m_window);

        SDL_GetWindowPosition(m_window, &m_PosX, &m_PosY);
        SDL_GetWindowSize(m_window, &m_Width, &m_Height);
        m_Fullscreen = (SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN) != 0;
        m_Exists = true;
        m_initialized = true;
        m_running = true;
        return true;
	}


    void Window::Update()
    {
        if (!m_initialized)
            return;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                m_running = false;
                m_Exists = false;
                continue;
            }

            switch (event.type)
            {
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                if (event.window.windowID == SDL_GetWindowID(m_window))
                {
                    m_running = false;
                    m_Exists = false;
                }
                break;

            case SDL_EVENT_WINDOW_MOVED:
                if (event.window.windowID == SDL_GetWindowID(m_window))
                {
                    m_PosX = event.window.data1;
                    m_PosY = event.window.data2;
                }
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                if (event.window.windowID == SDL_GetWindowID(m_window))
                {
                    m_Width = event.window.data1;
                    m_Height = event.window.data2;
                }
                break;

            default:
                break;
            }
        }
    }

    void Window::Close()
    {
        if (!m_initialized && !m_window && !m_glContext)
            return;

        if (m_glContext)
            SDL_GL_DestroyContext(m_glContext);
        if (m_window)
            SDL_DestroyWindow(m_window);
        if (m_initialized)
            SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);

        m_glContext = nullptr;
        m_window = nullptr;
        m_initialized = false;
        m_running = false;
        m_Exists = false;
    }


    void* Window::GetHandle()
    {
        return m_window;
    }
    bool Window::Exists()
    {
        return m_Exists;
    }
    int Window::GetPosX() { return m_PosX; }
    int Window::GetPosY() { return m_PosY; }
    int Window::GetWidth(){ return m_Width; }
    int Window::GetHeight() { return m_Height; }

    bool Window::GetFullscreen() { 
        return m_Fullscreen;
    }

    std::string Window::GetTitle(){
        return m_Title;
    }

    void Window::GetCurrentMonitorRect(int& left, int& top, int& right, int& bottom)
    {
        left = top = right = bottom = 0;
        if (!m_window)
            return;

        const SDL_DisplayID display = SDL_GetDisplayForWindow(m_window);
        SDL_Rect bounds{};
        if (display != 0 && SDL_GetDisplayBounds(display, &bounds))
        {
            left = bounds.x;
            top = bounds.y;
            right = bounds.x + bounds.w;
            bottom = bounds.y + bounds.h;
        }
    }

    void Window::SetPosition(int x, int y)
    {
        m_PosX = x;
        m_PosY = y;
        if (m_window)
            SDL_SetWindowPosition(m_window, m_PosX, m_PosY);
    }
    void Window::SetPosX(int x) 
    {
        SetPosition(x, m_PosY);
    }
    void Window::SetPosY(int y)
    {
        SetPosition(m_PosX, y);
    }
    void Window::SetSize(int w, int h)
    {
        m_Width = w;
        m_Height = h;
        if (m_window)
            SDL_SetWindowSize(m_window, m_Width, m_Height);
    }
    void Window::SetWidth(int w)
    {
        SetSize(w, m_Height);
    }
    void Window::SetHeight(int h)
    {
        SetSize(m_Width, h);
    }
    void Window::SetTitle(const char* title)
    {
        m_Title = title ? title : "";
        if (m_window)
            SDL_SetWindowTitle(m_window, m_Title.c_str());
    }
    void Window::SetFullscreen(bool enabled)
    {
        if (!m_window || m_Fullscreen == enabled)
            return;

        if (SDL_SetWindowFullscreen(m_window, enabled))
            m_Fullscreen = enabled;
    }
}
