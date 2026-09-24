#include "Editor.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "Engine/Window.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

Editor::Editor()
    : m_ui(m_assetDatabase)
{}

bool Editor::Initialize()
{
    clc::Window& window = clc::Window::GetInstance();
    window.SetTitle("Spin Editor");
    window.SetSize(1280, 800);
    if (!window.Initialize())
        return false;

    // Window owns both resources. Editor only borrows them to initialize and
    // render ImGui through its SDL/OpenGL backends.
    m_window = static_cast<SDL_Window*>(window.GetHandle());
    m_glContext = SDL_GL_GetCurrentContext();
    if (!m_window || !m_glContext)
    {
        window.Close();
        m_window = nullptr;
        m_glContext = nullptr;
        return false;
    }

    const SDL_DisplayID display = SDL_GetDisplayForWindow(m_window);
    const float displayScale = display != 0 ? SDL_GetDisplayContentScale(display) : 0.0f;
    const float mainScale = displayScale > 0.0f ? displayScale : 1.0f;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(mainScale);
    style.FontScaleDpi = mainScale;
    style.DockingSeparatorSize = 2.0f;

    ImGui_ImplSDL3_InitForOpenGL(m_window, m_glContext);
    ImGui_ImplOpenGL3_Init(nullptr);
    const std::filesystem::path assetsPath =
        std::filesystem::current_path()
        .parent_path()
        / "Spinblade"
        / "Assets";

    m_assetDatabase.OpenProject(assetsPath);
    m_ui.Initialize();
    m_initialized = true;
    return true;
}

void Editor::Run()
{
    if (!m_initialized)
        return;

    m_running = true;
    while (m_running)
    {
        ProcessEvents();
        if (!m_running)
            break;

        if (SDL_GetWindowFlags(m_window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        BeginFrame();

        m_ui.Draw();

        EndFrame();
    }
}

void Editor::ProcessEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT ||
            (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(m_window)))
        {
            m_running = false;
        }
    }
}

void Editor::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void Editor::EndFrame()
{
    ImGui::Render();
    const ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
    glClearColor(m_clearColor.x * m_clearColor.w, m_clearColor.y * m_clearColor.w, m_clearColor.z * m_clearColor.w, m_clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_window);
}

void Editor::Shutdown()
{
    if (!m_initialized)
        return;

    m_ui.Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    clc::Window::GetInstance().Close();

    m_glContext = nullptr;
    m_window = nullptr;
    m_initialized = false;
    m_running = false;
}
