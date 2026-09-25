/// ---------------------------------------------------------------------------
/// \file			AEXGfxSystem.h
/// \author			Thomas Komair
/// \brief			Declare the interface for the aex graphics engine
/// ---------------------------------------------------------------------------
#pragma once

// ---------------------------------------------------------------------------
// INCLUDES & DEPENDENCIES
#include "GL/AEXColor.h"
#include "GL/AEXTexture.h"
#include "GL/AEXShader.h"
#include "GL/AEXVertex.h"
#include "GL/AEXModel.h"
#include "../Engine/Singleton.h"
#include <GL/glew.h>
#include "AEXGfxDefs.h"
#include <list>
#include <glm/gtc/constants.hpp>
#include "../Engine/IBase.h"
#include "../Engine/Window.h"
#include "../Debug/DebugSystem.h"

#ifdef IN_LIBRARY
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif

namespace clc
{
	// Forward Declarations of Graphics related classes
	class Renderable;
	class Camera;

	// ---------------------------------------------------------------------------
	// CLASS: Graphics: Main system for the graphics engine
	class API Graphics : public IBase
	{
		CLC_SINGLETON(Graphics);
	public:

		// System functions
		bool Initialize();
		void Shutdown();
		void Render();
        // Render to the shared scene texture; caller owns presentation.
        void RenderScene(int width, int height);
        // Allows for editor camera to render
        void RenderScene(int width, int height, Camera* viewCamera);
        size_t GetCameraCount() const { return mCameras.size(); }

		#pragma region// Rendererer API

		SDL_Window* m_window = nullptr;
		Window* windowSys;
		void PresentFrame(GL::Model* pM, GL::ShaderProgram* pS);
		void InitFrameBuffer();
		void InitGBuffer();
		void ResetRenderer();
		void SetClearColor(const Color & col);
		void ClearFrameBuffer();
		void SetViewport(int left, int bottom, int right, int top);
		bool GetDepthTestEnabled();
		void SetDepthTestEnabled(bool enabled);
		bool GetVSyncEnabled();
		void SetVSyncEnabled(bool enabled);
		bool GetBlendingEnabled();
		void SetBlendingEnabled(bool enabled);
		void SetBlendMode(EBlendMode blendMode);
		EBlendMode GetBlendMode();

		#pragma endregion

		#pragma region// Component Management

		void AddRenderable(Renderable* renderable);
		void AddCamera(Camera* camera);
		void RemoveRenderable(Renderable* renderable);
		void RemoveCamera(Camera* camera);
		void ClearRenderables();
		void ClearCameras();
		void ClearComps();
		void SortCameras();
		Camera* GetCurrentCamera();
		unsigned int GetTexture() { return mColorBuffer; }
		
		#pragma endregion

		#pragma region// Debug Draw

		/*void DrawLine(AEVec2 p0, AEVec2 p1, Color col = clc::Color());
		void DrawLine(float x0, float y0, float x1, float y1, clc::Color col = clc::Color());
		void DrawRect(float x, float y, float w, float h, Color col = clc::Color());
		void DrawRectTwoCorners(AEVec2 p0, AEVec2 p1, Color col = clc::Color());
		void DrawOrientedRect(float x, float y, float w, float h, float angle, Color col = clc::Color());
		void DrawCircle(float cX, float cY, float radius, Color col = clc::Color(), float angle_start = 0, float angle_end = glm::pi<float>());
		void DrawLineData(bool clear = false);
		void ClearLineData();*/
		#pragma endregion

		std::list<Renderable*> mRenderables;
	private:

		// Renderer Properties
		bool						mbVSyncEnabled = true;
		EBlendMode					mCurrentBlendMode = EBlendMode::BM_ALPHA;

		// Line Renderer
		void InitLineRenderer();
		void FreeLineRenderer();
		GL::Model*			mLineMesh = nullptr;
		GL::ShaderProgram*	mLineShader = nullptr;
		unsigned			mLineVtxCount = 0;
		const unsigned		MAX_LINES = 2048;

		// Components
		std::list<Camera*> mCameras;
		Camera* mCurrentCamera = nullptr;

        bool ResizeSceneTarget(int width, int height);
        void RenderCamera(Camera* camera, int width, int height);
        int mTargetWidth = 0, mTargetHeight = 0;
		unsigned int mFrameBuffer = 0;
		unsigned int mColorBuffer = 0;
		unsigned int mDepthBuffer = 0;
		DebugSystem* debugSys = nullptr;

		unsigned int mSkyboxTexture = 0;
		GL::Model* mSkyboxModel = nullptr;
		GL::ShaderProgram* mSkyboxShader = nullptr;

		// gbuffer targets and the depth
		GLuint mGBuffer = 0;
		GLuint mGPosition = 0;
		GLuint mGNormal = 0;
		GLuint mGDiffuse = 0;
		GLuint mGDepth = 0;


		// Gui
		//bool InitializeGui();
		//void ShutdownGui();

		// This function should be called before calling any ImGui functions
		// it marks the start of a new frame of GUI and therefore should be placed 
		// at the beginning of the frame, after input has updated but before Logic updates. 
		// Note that NewFrame shouldn't be called anymore until EndFrame is called. Otherwise, 
		// ImGui will throw an error. 
		//void NewGuiFrame();

		// This function marks the end of the frame for ImGui. It will draw the GUI resulting
		// from all the ImGui calls that were made since NewFrame was last called. 
		// Because it will draw to the framebuffer, this function should be called just before
		// the graphics engine passes the framebuffer to the video display (i.e in aexGraphics->Present())
		//void EndGuiFrame();
	};
}
// Easy access to singleton
#define clcGraphics (clc::Graphics::GetInstance())
