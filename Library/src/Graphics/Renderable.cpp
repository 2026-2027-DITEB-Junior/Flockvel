#include "pch.h"
#include "GfxSystem.h"
#include "Renderable.h"
#include "GL/AEXShader.h"
//#include <iostream>
#include "AEXGfxDefs.h"
#include "../Resources/ResourceManager.h"
#include "../Engine/Serialization.h"
#include <stdexcept>

namespace clc {

	Renderable::Renderable() { texPath = "default.png"; modelPath = std::string{ "cube_face.obj" }; shaderPath = "TextureMap.shader"; };
	Renderable::Renderable(std::string modelName) : Renderable() { modelPath = modelName; };

	Renderable::Renderable(const Renderable* ren)
	{
		//transform = ;
		texPath = ren->texPath;
		modelPath = ren->modelPath;
		shaderPath = ren->shaderPath;
		mpMesh = ren->mpMesh;
		mpShader = ren->mpShader;
		mpTexture = ren->mpTexture;
	}


	// IComp API
	void Renderable::OnCreate()
	{
        transform = nullptr;
        if (mOwner != nullptr)
            transform = &mOwner->mTransform;

        mpMesh = clcResources.GetModel(modelPath);
        mpTexture = nullptr;
        if (!texPath.empty())
            mpTexture = clcResources.GetTexture(texPath);
        mpShader = clcResources.GetShader(shaderPath);

        bool missingTexture = !texPath.empty() && mpTexture == nullptr;
        if (mpMesh == nullptr || mpShader == nullptr || missingTexture)
        {
            std::string objectName = "debug renderable";
            if (mOwner != nullptr)
                objectName = mOwner->mName;
            throw std::runtime_error("Missing or invalid render resources on object: " + objectName);
        }
	}
	void Renderable::AddToSystem(){
		clcGraphics.AddRenderable(this);
	}
	void Renderable::RemoveFromSystem(){
		clcGraphics.RemoveRenderable(this);
	}

	/* RENDERABLE BASE API (OVERLOAD THOSE).
	*  Each step of the Renderable Render function is customizable, Render is implemented as:
	*  void Render(){
	*		BindResources(true){}
	*		PreDraw(){}
	*		DrawMesh(){}
	*		PostDraw(){}
	*		BindResources(false){}
	*/
	void Renderable::Render(){
		if (!mpMesh || !mpShader) {
			/*DebugPrint("Renderable(%s): invalid -> mesh=%p shader=%p tex=%p \n",
				Owner()->GetName().c_str(), mpMesh, mpShader, mpTexture);*/
			return;
		}
		/*if (!ValidateResources()) {
			DebugPrint("Warning Object(%s): Renderable::Render(): Resources invalid!", Owner()->GetName().c_str());
			return;
		}*/

		BindResources(true);
		PreDraw();	
		DrawMesh();
		PostDraw(); 
		BindResources(false);
	}
	void Renderable::BindResources(bool bind) {

		// sanity check-> we have the minimum resources to draw:
		if (!ValidateResources()) return;

		// bind step
		if (bind) {

			// set blend mode
			clcGraphics.SetBlendMode(mBlendMode);

			// bind model and shaders
			if (mpMesh)mpMesh->Bind();
			if (mpShader) {
				//mpShader->Bind();

				// pass model to world matrix to the shader
                glm::mat4 modelMatrix(1.f);
                if (Owner() != nullptr)
                    modelMatrix = Owner()->GetWorldMatrix();
                else if (transform != nullptr)
                    modelMatrix = transform->WorldMtx();
                mpShader->SetUniform("mtxModel", modelMatrix);

				// bind texture
				if (mpTexture) {
					// change this to add more textures
					int texture_unit = 0;
					mpTexture->Bind(texture_unit);
					mpShader->SetUniform("texSampler", texture_unit);
				}
			}
		}
		// unbind step
		else {
			if (mpMesh)mpMesh->Unbind();
			//if (mpShader)mpShader->Unbind();
			if (mpTexture)mpTexture->Unbind();
		}
	}
	void Renderable::DrawMesh(){
		// call draw on the model.
		if(mpMesh)mpMesh->Draw();
	}
	bool Renderable::ValidateResources()
	{
		return mpMesh && mpShader;
	}

	void Renderable::ShowInEditor()
	{
		
	}

	void Renderable::Serialize(nlohmann::json& j) const
	{
		IBase::Serialize(j); // type, name and enabled

		j["texture"] = texPath;
		j["model"] = modelPath;
		j["shader"] = shaderPath;
		j["color"] = mModulationColor;
		j["tintOnly"] = mbTintOnly;
		j["blendMode"] = mBlendMode;
	}

	void Renderable::Deserialize(const nlohmann::json& j)
	{
		IBase::Deserialize(j);

		// OnCreate asks the manager for these
		if (j.contains("texture")) texPath = j["texture"].get<std::string>();
		if (j.contains("model")) modelPath = j["model"].get<std::string>();
		if (j.contains("shader")) shaderPath = j["shader"].get<std::string>();
		if (j.contains("color")) j["color"].get_to(mModulationColor);
		if (j.contains("tintOnly")) mbTintOnly = j["tintOnly"].get<bool>();
		if (j.contains("blendMode")) mBlendMode = j["blendMode"].get<EBlendMode>();
	}

	Renderable* Renderable::Clone() { return new Renderable(*this); }

	bool Renderable::operator== (const Renderable& ren)
	{
		if (mpTexture != ren.mpTexture)
			return false;

		if (mpMesh != ren.mpMesh)
			return false;

		if (mpShader != ren.mpShader)
			return false;

		if (mModulationColor != ren.mModulationColor)
			return false;

		if (mbTintOnly != ren.mbTintOnly)
			return false;

		if (mBlendMode != ren.mBlendMode)
			return false;

		if (mFlipX != ren.mFlipX)
			return false;

		if (mFlipY != ren.mFlipY)
			return false;

		return true;
	}

	bool Renderable::operator!= (const Renderable& ren)
	{
		return !(*this == ren);
	}

	void Renderable::PreDraw() {}
	void Renderable::PostDraw() {}
}