/// -------------------------------------------------------------------
/// \file			AEXGfxSystem.cpp
/// \author			Thomas Komair
/// \brief			Implementation of the aex graphics engine
/// -------------------------------------------------------------------

// ---------------------------------------------------------------------------
// INCLUDES & DEPENDENCIES
#include "pch.h"
#include "GfxSystem.h"
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/GL.h>
#include "Camera.h"
#include "Renderable.h"
#include "GL/AEXGL.h"
#include "glm/glm.hpp"
#include "../Resources/ResourceManager.h"

// ---------------------------------------------------------------------------

namespace clc
{
	// GDI Device context and GL Render context
	static GLint	gMajorVersion = 4;
	static GLint	gMinorVersion = 2;

	static GLuint CreateRenderTarget(int width, int height, GLenum internalFormat, GLenum format, GLenum type) {
		GLuint texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);

		// no mipmaps and no wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0); // unbind

		return texture;
	}

	// ---------------------------------------------------------------------------
	bool Graphics::Initialize()
	{
		// Get main window Win32 handle
		m_window = static_cast<SDL_Window*>(clcWindow.GetHandle());
		windowSys = &clcWindow;
		debugSys = &clcDebug;

		// Initialize OpenGL
		//if (!GL::InitializeOpenGL((HWND)m_window, gMajorVersion, gMinorVersion)) return false;

		InitFrameBuffer();
        if (!mSkyboxTexture || !mSkyboxModel || !mSkyboxShader) return false;

		// reset the render state
		//ResetRenderer();

		// Create line renderer
		InitLineRenderer();

		// Init Gui
		//InitializeGui();

		// No errors
		return true;
	}

	// ---------------------------------------------------------------------------
	void Graphics::Shutdown()
	{
		//ShutdownGui();
		FreeLineRenderer();
		ClearComps();
        if (mFrameBuffer) glDeleteFramebuffers(1, &mFrameBuffer);
        if (mColorBuffer) glDeleteTextures(1, &mColorBuffer);
        if (mDepthBuffer) glDeleteTextures(1, &mDepthBuffer);
        if (mSkyboxTexture) glDeleteTextures(1, &mSkyboxTexture);
        mFrameBuffer = mColorBuffer = mDepthBuffer = mSkyboxTexture = 0;
        mTargetWidth = mTargetHeight = 0;
        mSkyboxModel = nullptr;
        mSkyboxShader = nullptr;
        if (debugSys) debugSys->Clear();
        m_window = nullptr; // Window owns the SDL OpenGL context.
	}

	void Graphics::PresentFrame(GL::Model* pM, GL::ShaderProgram* pS)
	{
		if (!pM || !pS) return;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        int width = 1, height = 1;
        SDL_GetWindowSizeInPixels(m_window, &width, &height);
        SetViewport(0, 0, width, height);
        SetClearColor(Color(1.0f, 0.0f, 1.0f, 1));
        ClearFrameBuffer();

		pS->Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mColorBuffer);
		pS->SetUniform("scene", 0);
		pS->SetUniform("size", glm::vec2(2.0f, 2.0f));
		pM->Bind();
		pM->Draw();
		pM->Unbind();
		glBindTexture(GL_TEXTURE_2D, 0);
		pS->Unbind();

		SDL_GL_SwapWindow(m_window);
	}

    void Graphics::Render()
    {
        int width = 1, height = 1;
        SDL_GetWindowSizeInPixels(m_window, &width, &height);
        RenderScene(width, height);
    }

    void Graphics::RenderScene(int width, int height)
    {
        RenderScene(width, height, nullptr);
    }

    void Graphics::RenderScene(int width, int height, Camera* viewCamera)
    {
        if (width <= 0 || height <= 0 || !ResizeSceneTarget(width, height)) return;
        glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
        ResetRenderer();
        SetViewport(0, 0, width, height);
        SetClearColor(Color(0.4f, 0.4f, 0.4f, 1));
        ClearFrameBuffer();

        if (viewCamera != nullptr)
        {
            RenderCamera(viewCamera, width, height);
        }
        else
        {
            SortCameras();
            for (Camera* camera : mCameras)
                RenderCamera(camera, width, height);
        }

        mLineVtxCount = 0;
        mCurrentCamera = nullptr;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        debugSys->Clear();
    }

    void Graphics::RenderCamera(Camera* cam, int width, int height)
    {
        if (cam == nullptr || cam->Owner() == nullptr || cam->IsShutdown()
            || !cam->IsEnabled() || !cam->Owner()->IsActiveInHierarchy()) return;
        mCurrentCamera = cam;

        mCurrentCamera->mViewRectangle = glm::vec2(width, height);
        //cam->ApplyViewport();
        auto viewProj = cam->ViewProjMtx44();

        // TODO: tidy up the code
        {
            mSkyboxShader->Bind();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, mSkyboxTexture);

            mSkyboxShader->SetUniform("mtxView", cam->ViewMtx44());
            mSkyboxShader->SetUniform("mtxProj", cam->ProjectionMtx44());
            mSkyboxShader->SetUniform("mtxModel", glm::scale(glm::mat4(1.0f), glm::vec3(5, 5, 5)));
            mSkyboxShader->SetUniform("skyboxTex", 0);

            mSkyboxModel->Bind();
            mSkyboxModel->Draw();
            mSkyboxModel->Unbind();
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            mSkyboxShader->Unbind();
            glClear(GL_DEPTH_BUFFER_BIT);
        }

        for (auto ren : mRenderables)
        {
            // is the renderable enabled
            if (ren->IsShutdown() || !ren->IsEnabled() || !ren->Owner()->IsActiveInHierarchy()) continue;

            // set camera matrix in the shader
            if (ren->mpShader) {
                ren->mpShader->Bind();
				ren->mpShader->SetUniform("projection", viewProj);
				ren->mpShader->SetUniform("diffuseTex", 0);
				ren->mpShader->SetUniform("specularTex", 1);
				ren->mpShader->SetUniform("normalTex", 2);
            }

            for (auto mod : ren->modifiers)
            {
                if (!mod->enabled)
                    continue;
                mod->PreRender();
            }

            // render
            ren->Render();

            for (auto mod : ren->modifiers)
            {
                if (!mod->enabled)
                    continue;
                mod->PostRender();
            }
        }

        glDisable(GL_CULL_FACE);
        for (auto& obj : debugSys->allObjects)
        {
            if (!obj.ren)
                continue;
            // set camera matrix in the shader
            if (obj.ren->mpShader) {
                obj.ren->mpShader->Bind();
                obj.ren->mpShader->SetUniform("mtxViewProj", viewProj);
                obj.ren->mpShader->SetUniform("drawColor", obj.col);
            }

            obj.ren->transform = &obj.trs;
            // render
            obj.ren->Render();
        }
        glEnable(GL_CULL_FACE);
    }

    bool Graphics::ResizeSceneTarget(int width, int height)
    {
        if (mFrameBuffer && width == mTargetWidth && height == mTargetHeight) return true;
        if (!mColorBuffer) glGenTextures(1, &mColorBuffer);
        if (!mDepthBuffer) glGenTextures(1, &mDepthBuffer);
        if (!mFrameBuffer) glGenFramebuffers(1, &mFrameBuffer);
        glBindTexture(GL_TEXTURE_2D, mColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, mDepthBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorBuffer, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthBuffer, 0);
        const bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (complete) { mTargetWidth = width; mTargetHeight = height; }
        return complete;
    }

    void Graphics::InitFrameBuffer()
    {
        if (!ResizeSceneTarget(clcWindow.GetWidth(), clcWindow.GetHeight())) return;
		#pragma region SKYBOX
		glGenTextures(1, &mSkyboxTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mSkyboxTexture);

		auto& rsc = clcResources;
		std::vector<GL::Texture*> textures = rsc.GetCubeTexture("skybox_.png");
        if (textures.size() != 6) { glBindTexture(GL_TEXTURE_CUBE_MAP, 0); return; }
		for (size_t i = 0; i < 6; i++)
		{
			//GL::Texture* cubeFace = rsc.GetTexture("skybox_" + std::to_string(i + 1) + ".png");
			GL::Texture* cubeFace = textures[i];
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, cubeFace->GetWidth(),
				cubeFace->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, cubeFace->GetPixels());
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		mSkyboxModel = rsc.GetModel("skybox.obj");
		mSkyboxShader = rsc.GetShader("Skybox.shader");
		#pragma endregion
	}

	void Graphics::InitGBuffer() {
		glGenFramebuffers(1, &mGBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer);

		// pos and normals need floats
		mGPosition = CreateRenderTarget(mTargetWidth, mTargetHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT);
		mGNormal = CreateRenderTarget(mTargetWidth, mTargetHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT);
		mGDiffuse = CreateRenderTarget(mTargetWidth, mTargetHeight, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
		mGDepth = CreateRenderTarget(mTargetWidth, mTargetHeight, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGPosition, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mGNormal, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mGDiffuse, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mGDepth, 0);

		// the 3 of them are written on the same pass
		GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Error: GBuffer framebuffer is not complete." << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// ------------------------------------------------------------------------
	#pragma region// RENDER STATE

	void Graphics::ResetRenderer()
	{
		// disable backface removal
		glEnable(GL_CULL_FACE);

		// Enable blending
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        mCurrentBlendMode = BM_ALPHA;

		// Select which texture unit subsequent texture state calls will affect. 
		// by default, all textures will be applied on texture unit 0
		glActiveTexture(GL_TEXTURE0);

		// enable depth testing
		glEnable(GL_SCISSOR_TEST);
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.0);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0f, 1.0f);
	}
	void Graphics::SetViewport(int left, int bottom, int right, int top)
	{
		glScissor(left, bottom, abs(right - left), abs(top - bottom));
		glViewport(left, bottom, abs(right-left), abs(top-bottom));
	}
	void Graphics::SetClearColor(const Color & col)
	{
		glClearColor(col.r, col.g, col.b, col.a); // set clear color to black
	}
	void Graphics::ClearFrameBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	bool Graphics::GetDepthTestEnabled()
	{
		GLboolean enabled;
		glGetBooleanv(GL_DEPTH_TEST, &enabled);
		return enabled != 0;
	}
	void Graphics::SetDepthTestEnabled(bool enabled)
	{
		bool currentEnabled = GetDepthTestEnabled();
		if (currentEnabled == enabled)
			return;
		if (enabled == true)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}
	bool Graphics::GetVSyncEnabled()
	{
		return mbVSyncEnabled;
	}
	void Graphics::SetVSyncEnabled(bool enabled)
	{
		if (enabled == mbVSyncEnabled)return;
		mbVSyncEnabled = enabled;
		wglSwapIntervalEXT(enabled ? 1 : 0);
	}

	bool Graphics::GetBlendingEnabled()
	{
		GLboolean enabled;
		glGetBooleanv(GL_BLEND, &enabled);
		return enabled != 0;
	}
	void Graphics::SetBlendingEnabled(bool enabled)
	{
		enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	}
	void Graphics::SetBlendMode(EBlendMode blendMode)
	{
		if (mCurrentBlendMode == blendMode)return;
		mCurrentBlendMode = blendMode;
		switch (blendMode)
		{
		case BM_ALPHA:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case BM_ADDITIVE:
			glBlendFunc(GL_ONE, GL_ONE);
			break;
		case BM_MULTIPLY:
			glBlendFuncSeparate(
				GL_DST_COLOR, GL_ZERO,
				GL_DST_ALPHA, GL_ZERO);
			break;
		case BM_REPLACE:
			glBlendFunc(GL_ONE, GL_ZERO);
			break;
		}
	}

	EBlendMode Graphics::GetBlendMode()
	{
		return mCurrentBlendMode;
	}

	#pragma endregion
	
	
	// ------------------------------------------------------------------------
	#pragma region// COMPONENT MANAGEMENT

	void Graphics::AddRenderable(Renderable* renderable){
		for (auto ren : mRenderables)
			if (ren == renderable)return;
		mRenderables.push_front(renderable);
	}
	void Graphics::AddCamera(Camera* camera){
		for (auto ren : mCameras)
			if (ren == camera)return;
		mCameras.push_back(camera);
	}
	void Graphics::RemoveRenderable(Renderable* renderable){
		mRenderables.remove(renderable);
	}
	void Graphics::RemoveCamera(Camera* camera){
		mCameras.remove(camera);
        if (mCurrentCamera == camera) mCurrentCamera = nullptr;
	}
	void Graphics::ClearRenderables(){
		mRenderables.clear();
	}
	void Graphics::ClearCameras(){
		mCameras.clear();
        mCurrentCamera = nullptr;
	}
	void Graphics::ClearComps(){
		ClearRenderables();
		ClearCameras();
		mCurrentCamera = nullptr;
	}
	void Graphics::SortCameras()
	{
		mCameras.sort([](Camera* l, Camera* r) {
			return l->mDrawOrder < r->mDrawOrder;
			});
	}

	Camera* Graphics::GetCurrentCamera()
	{
		return mCurrentCamera;
	}

	#pragma endregion

	// ------------------------------------------------------------------------
	#pragma region// DEBUG DRAWING METHODS

	void Graphics::InitLineRenderer()
	{
		if (!mLineMesh) {
			mLineMesh = new GL::Model;
			mLineMesh->SetPrimitiveType(GL::Model::eLineList);
			for (unsigned i = 0; i < MAX_LINES * 2; ++i)
				mLineMesh->AddVertex(GL::Vertex());
			mLineMesh->UploadToGPU();
		}

		if (!mLineShader) {
			//mLineShader = new GL::ShaderProgram("data/shaders/Line.shader");
		}
	}
	void Graphics::FreeLineRenderer()
	{
		if (mLineMesh)
		{
			delete mLineMesh;
			mLineMesh = NULL;
		}
		mLineShader = NULL;
	}
	//void Graphics::DrawLine(glm::vec3 p0, glm::vec3 p1, Color col)
	//{
	//	if (mLineVtxCount >= (MAX_LINES * 2))
	//		return;

	//	mLineMesh->SetVertexPos(mLineVtxCount, p0);
	//	mLineMesh->SetVertexColor(mLineVtxCount++, col);
	//	mLineMesh->SetVertexPos(mLineVtxCount, p1);
	//	mLineMesh->SetVertexColor(mLineVtxCount++, col);
	//}
	//void Graphics::DrawLine(float x0, float y0, float x1, float y1, Color col)
	//{
	//	DrawLine(AEVec2(x0, y0), AEVec2(x1, y1), col);
	//}
	//void Graphics::DrawRect(float x, float y, float w, float h, Color c)
	//{
	//	AEVec2 p = { x,y };
	//	AEVec2 s = { w,h };

	//	clc::AEVec2 top_left = p + clc::AEVec2(-s.x / 2.0f, s.y / 2.0f);
	//	clc::AEVec2 top_right = p + clc::AEVec2(s.x / 2.0f, s.y / 2.0f);
	//	clc::AEVec2 bot_left = p + clc::AEVec2(-s.x / 2.0f, -s.y / 2.0f);
	//	clc::AEVec2 bot_right = p + clc::AEVec2(s.x / 2.0f, -s.y / 2.0f);

	//	DrawLine(top_left, top_right, c);
	//	DrawLine(top_right, bot_right, c);
	//	DrawLine(bot_left, bot_right, c);
	//	DrawLine(bot_left, top_left, c);
	//}
	//void Graphics::DrawRectTwoCorners(AEVec2 p0, AEVec2 p1, Color c)
	//{
	//	AEVec2 top_left = {
	//	(p0.x < p1.x) ? p0.x : p1.x,
	//	(p0.y < p1.y) ? p0.y : p1.y
	//	};

	//	AEVec2 bot_right = {
	//		(p0.x > p1.x) ? p0.x : p1.x,
	//		(p0.y > p1.y) ? p0.y : p1.y
	//	};

	//	AEVec2 top_right = {
	//		bot_right.x,
	//		top_left.y
	//	};

	//	AEVec2 bot_left = {
	//		top_left.x,
	//		bot_right.y
	//	};

	//	DrawLine(top_left, top_right, c);
	//	DrawLine(top_right, bot_right, c);
	//	DrawLine(bot_left, bot_right, c);
	//	DrawLine(bot_left, top_left, c);
	//}
	//void Graphics::DrawOrientedRect(float x, float y, float w, float h, float angle, Color col)
	//{
	//	AEVec2 axis_x; axis_x.FromAngle(angle);
	//	AEVec2 axis_y = axis_x.Perp();

	//	axis_x *= w * 0.5f;
	//	axis_y *= h * 0.5f;

	//	// get corners
	//	AEVec2 pos = { x, y };
	//	AEVec2 tR = pos + axis_x + axis_y;
	//	AEVec2 tL = pos - axis_x + axis_y;
	//	AEVec2 bR = pos + axis_x - axis_y;
	//	AEVec2 bL = pos - axis_x - axis_y;

	//	// draw as obb
	//	DrawLine(tR, tL);
	//	DrawLine(tL, bL);
	//	DrawLine(bL, bR);
	//	DrawLine(bR, tR);
	//}
	//void Graphics::DrawCircle(f32 cX, f32 cY, f32 radius, Color col, f32 angle_start, f32 angle_end)
	//{
	//	AEVec2 center = { cX, cY };
	//	f32 iterations = 24;
	//	if (angle_start > angle_end)
	//		angle_end += TWO_PI;

	//	f32 angle = angle_start;
	//	f32 angle_range = angle_end - angle_start;
	//	f32 angle_inc = angle_range / (f32)iterations;
	//	AEVec2 v1, v0;
	//	for (u32 i = 0; i < iterations; ++i, angle += angle_inc)
	//	{
	//		// TODO(Thomas): This is pretty slow... 
	//		v0 = center + AEVec2(cos(angle), sin(angle)) * radius;
	//		v1 = center + AEVec2(cos(angle + angle_inc), sin(angle + angle_inc)) * radius;
	//		DrawLine(v0, v1, col);
	//	}
	//}
	//void Graphics::DrawLineData(bool clear)
	//{
	//	if (!mLineMesh || !mLineShader || !mLineVtxCount)
	//		return;

	//	// Store renderer state for later. Set the desired values.
	//	bool blendEnabled = GetBlendingEnabled(); SetBlendingEnabled(false);
	//	EBlendMode blendMode = GetBlendMode(); SetBlendMode(clc::BM_REPLACE);
	//	bool depthEnabled = GetDepthTestEnabled(); SetDepthTestEnabled(false);

	//	// set shader model matrix to identity
	//	if (mLineShader)
	//	{
	//		AEMtx44 view_proj;
	//		if (mCurrentCamera == nullptr) {
	//			// default values
	//			auto w = aexWindow.GetWidth();
	//			auto h = aexWindow.GetHeight();
	//			auto proj = AEMtx44::OrthoProjGL(f32(w), f32(h), 0.01f, 1000.f);
	//			auto view = AEMtx44::Translate(0, 0, -20);
	//			view_proj = proj * view;
	//		}
	//		else
	//			view_proj = mCurrentCamera->ViewProjMtx44();

	//		mLineShader->Bind();
	//		mLineShader->SetUniform("mtxViewProj", view_proj);
	//		mLineShader->SetUniform("mtxModel", AEMtx44::Identity());
	//	}

	//	// Upload the data
	//	mLineMesh->ReloadToGPU(0, mLineVtxCount);

	//	// Draw
	//	mLineMesh->Draw(0, mLineVtxCount);

	//	// reset the number of vertices
	//	if (clear)
	//		ClearLineData();

	//	// reset blend state
	//	SetBlendingEnabled(blendEnabled);
	//	SetBlendMode(blendMode);
	//	SetDepthTestEnabled(depthEnabled);
	//}

	//void Graphics::ClearLineData()
	//{
	//	mLineVtxCount = 0;
	//}

	#pragma endregion

}// namespace aex
