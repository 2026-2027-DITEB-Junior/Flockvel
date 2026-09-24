#pragma once
#include "EditorUI.h"
#include "imgui.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

class Editor
{
public:
    Editor();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    void ProcessEvents();
    void BeginFrame();
    void EndFrame();

    AssetDatabase m_assetDatabase;
    EditorUI m_ui;
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_glContext = nullptr;
    bool m_initialized = false;
    bool m_running = false;
    bool m_showDemoWindow = true;
    float m_value = 0.0f;
    int m_counter = 0;
    ImVec4 m_clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};