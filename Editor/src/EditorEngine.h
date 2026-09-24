#pragma once
#include <list>
#include "Engine/Engine.h"
#include "GameExecution.h"

#include "../EditorUI.h"
#include <imgui.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

namespace clc
{
	class EditorEngine : public EngineSystem
    {
	public:
        EditorEngine(const EditorEngine&) = delete;
        EditorEngine& operator=(const EditorEngine&) = delete;

		bool SpecificInitialize() override;
        bool RendersOwnFrame() const override { return true; }
		void SpecificRun() override;
		void SpecificShutdown() override;

        static EditorEngine& GetInstance();

    private:
        EditorEngine() : m_ui(m_assetDatabase) {}
        ~EditorEngine() {}

        void ProcessEvents();
        void BeginFrame();
        void EndFrame();

        ExecutionSystem* exe = nullptr;

        AssetDatabase m_assetDatabase;
        EditorUI m_ui;

        bool m_showDemoWindow = true;
        float m_value = 0.0f;
        int m_counter = 0;
        ImVec4 m_clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	};
}

#define clcEngine (clc::EditorEngine::GetInstance())