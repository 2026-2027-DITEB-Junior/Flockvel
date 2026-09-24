#pragma once
#include "../Engine/IComp.h"
#include "GL/AEXColor.h"
#include "GL/AEXTexture.h"
#include "GL/AEXShader.h"
#include "GL/AEXVertex.h"
#include "GL/AEXModel.h"
#include "../Engine/Transform.h"
#include "tiny_obj_loader.h"
#include "AEXGfxDefs.h"
#include "RenderModifier.h"

#ifdef IN_LIBRARY
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif

namespace clc {

	class API Renderable : public IComp {
		friend class Graphics;

	protected:

		// Pointer to transform component used to compute the model-to-world matrix. 
		// Note: this component is required on the owner object. 
		// if it can't be found, an assert error will be triggered.
		Transform* transform = nullptr;	
	public:

		//----------------------------------------

		std::string texPath;
		std::string modelPath;
		std::string shaderPath;
		Renderable();
		Renderable(std::string modelName);
		Renderable(const Renderable* ren);

		// 
		// Graphics resources needed to render properly.
		// Note: Those are never loaded in the Renderable. 
		// Instead, they are set during the loading process.

		// model containing the vertex data. Required for rendering
		GL::Model* mpMesh = nullptr;				

		// shader program used to draw the model. Required for rendering
		GL::ShaderProgram* mpShader = nullptr;		

		// texture (image) to apply when. Optional, if null, shader will draw the vertex colors. 
		GL::Texture* mpTexture = nullptr;		

		std::vector<RenderModifier*> modifiers;

		// 
		// Shading properties

		// Modulation color. Multiply each rendered pixel by this color. 
		// Default is white (1,1,1,1) which will result in the pixel
		// color being unchanged
		Color mModulationColor = Color(1, 1, 1, 1);			

		// If this is true, then modulation color will 
		// replace all pixels output by rendering.
		bool  mbTintOnly = false;

		// Blend mode when rendering this component. calls Graphics::SetBlendMode().
		// default is alpha blending
		EBlendMode mBlendMode = BM_ALPHA;

		bool mFlipX = false;
		bool mFlipY = false;

		Renderable* Clone() override;

		bool operator== (const Renderable& ren);

		bool operator!= (const Renderable& ren);

		// 
		// IComp API
		void OnCreate()override;
		void AddToSystem()override;
		void RemoveFromSystem()override;

		// 
		// Renderable API.

		//	Base Render function. (called by Graphics system)
		virtual void Render();
			// NOTE: Each step of this function is overridable (see below)
			// Even this one it is implemented as:
			//  void Render(){
			//		BindResources(true);
			//		PreDraw();
			//		DrawMesh();
			//		PostDraw();
			//		BindResources(false);
			//
		virtual void BindResources(bool bind);
		virtual void DrawMesh();

		// these don't do anything by default. override for custom render effects. 
		virtual void PreDraw();
		virtual void PostDraw();

		// returns true if pShader and pMesh are valid pointers (i.e not null)
		virtual bool ValidateResources();

		virtual void ShowInEditor();

		void Serialize(nlohmann::json& j) const override;
		void Deserialize(const nlohmann::json& j) override;
	};
}