#include "EditorEngine.h"
#include <iostream>

#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <Engine/Window.h>

namespace clc
{
	bool EditorEngine::SpecificInitialize()
	{
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

        exe = &clcExe;
        if (!exe->Initialize()) return false;

		std::cout << "Editor still compiles! :]\n";
		return true;
	}

	void EditorEngine::SpecificRun()
	{
        ProcessEvents();
        if (!m_running)
            return;

        if (SDL_GetWindowFlags(m_window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            return;
        }

        BeginFrame();

        m_ui.Draw();
        if (m_ui.WantsToClose()) m_running = false;

        EndFrame();
	}

	void EditorEngine::SpecificShutdown()
	{
        if (!m_initialized)
            return;

        m_ui.Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();


        m_glContext = nullptr;
        m_window = nullptr;
        m_initialized = false;
        m_running = false;

        if (exe) exe->Shutdown();
	}

    EditorEngine& EditorEngine::GetInstance()
    {
        static EditorEngine instance;
        return instance;
    }

    void EditorEngine::ProcessEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT ||
                (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(m_window)))
            {
                m_ui.RequestClose();
            }
        }
    }

    void EditorEngine::BeginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void EditorEngine::EndFrame()
    {
        ImGui::Render();
        const ImGuiIO& io = ImGui::GetIO();
        int width = 1, height = 1;
        SDL_GetWindowSizeInPixels(m_window, &width, &height);
        glViewport(0, 0, width, height);
        glDisable(GL_SCISSOR_TEST);
        glClearColor(m_clearColor.x * m_clearColor.w, m_clearColor.y * m_clearColor.w, m_clearColor.z * m_clearColor.w, m_clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(m_window);
    }
}