#pragma once
#include "GL/AEXColor.h"
#include "../Engine/IComp.h"
#include "../Engine/Transform.h"

#ifdef IN_LIBRARY
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif

namespace clc {
	
	class API Camera : public IComp {
		friend class Graphics;

	protected:
		// Pointer to transform component used to compute view matrix
		// Note: this component is required on the owner object. 
		// if it can't be found, an assert error will be triggered.
		
	public: 
		Transform* transform = nullptr;
		glm::vec3 target = glm::vec3(0.f);
		glm::vec3 upVec;
		float nearPlane = 1.f;
		float farPlane = 500.f;
		// Camera Properties
		// NOTE: These properties are made public for simplicity's sake. 
		// Since they are used every frame when rendering, it's fine to 
		// change them at will, as the changes will take effect in that frame.
		
		// size of the the view volume (width, height) for the orthographic projection (1920.0f, 1080.0f)
		glm::vec2 mViewRectangle = glm::vec2(1920.0f, 1080.0f);
		float fov = 60.f;

		// Viewport Data
		glm::vec2 mViewportBL;				// bottom-left position of the viewport	in normalized coordinates	
		glm::vec2 mViewportTR;				// top-right position of the viewport in normalized coordinates
		bool mbClearViewport = true;	// should clear the viewport or not?
		Color mViewportColor;			// if so, use this color to clear the viewport.  

		// Draw Order. Used by the Graphics when there are multiple cameras
		// in the scene to determine in which order to draw them (see Graphics::Render)
		// They are sorted in ascending order. I.e. lower draw orders draw first.
		int mDrawOrder = 0;

	public:

		Camera* Clone();

		//
		// IComp inherited

		// adds itself to the graphics system by calling aexGraphics.AddCamera()
		void AddToSystem()override;

		// removes itself from the graphics system by calling aexGraphics.RemoveCamera()
		void RemoveFromSystem() override;

		// On creation, the camera fetches the transform component. 
		// if none is found, it should error
		void OnCreate()override;

		// 
		// Camera specific

		// Set the camera viewport properties to the renderer
		// Sets the viewport dimensions using aexGraphics.SetViewport()
		// If mbClearViewport is true then:
		//	1. We set the clear color using aexGraphics.SetClearColor()
		//	2. We clear the framebuffer using aexGraphics.ClearBuffer()
		void ApplyViewport()const;

		// get view matrix
		// The view matrix represents an inverse model to world transform
		// because it transform any point from the world back in to the
		// camera space. 
		// These should be computed using the transform component, such that:
		// view_mtx = transform->InvWorldMtx();
		// inv_view_mtx = transform->WorldMtx();
		glm::mat4 ViewMtx44()const;
		glm::mat4 InvViewMtx44()const;
		
		// get projection matrix
		// this function calls AEMtx44::OrthoProjGL with the values of
		// the viewrectangle for with and height. For near and far, just use
		glm::mat4 ProjectionMtx44()const;

		// get model view projection matrix;
		// helper function that simply concatenates the matrices returned
		// by ProjectionMtx44() and ViewMtx44 in that order:
		// viewproj = proj * view;
		glm::mat4 ViewProjMtx44()const;

		// point conversion
		// Helper functions to convert a point from the different
		// coordinate systems we usually work in: window, viewport, world. 

		// Converts a point in window to world coordinates
		glm::vec2 WindowPointToWorld(const glm::vec2& wp)const;

		// Converts a point from world to window coordinates
		glm::vec2 WorldPointToWindow(const glm::vec2& wp)const;

		// Converts a point from world to viewport coordinates
		glm::vec2 WorldPointToViewport(const glm::vec2& wp)const;

		// Checks if a point in window coordinates is inside the camera viewport
		bool WindowPointInViewport(const glm::vec2& wp)const;

		// Checks if a point in world coordinates is inside the camera viewport
		bool WorldPointInViewport(const glm::vec2& wp)const;

		// Converts the mouse position to world using the function
		// WindowPointToWorld() above. 
		glm::vec2 MouseToWorld()const;

		// Checks if the mouse is inside the viewport
		// WindowPointInViewport above
		bool MouseInViewport()const;

		void Serialize(nlohmann::json& j) const override;
		void Deserialize(const nlohmann::json& j) override;

		bool operator== (const Camera& cam);

		bool operator!= (const Camera& cam);

		bool Equal(const IComp& comp);
	};
}