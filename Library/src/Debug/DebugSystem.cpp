#include "pch.h"
#include "DebugSystem.h"
#include "../Resources/ResourceManager.h"
#include "../Graphics/Renderable.h"

namespace clc
{
	void DebugSystem::Initialize()
	{
		sphere = new Renderable();
		sphere->modelPath = "debugSphere.obj";
		sphere->shaderPath = "DebugDraw.shader";
		sphere->OnCreate();
		sphere->mpMesh->SetDrawMode(GL::Model::eDM_Wireframe);

		cube = new Renderable();
		cube->modelPath = "debugCube.obj";
		cube->shaderPath = "DebugDraw.shader";
		cube->OnCreate();
		cube->mpMesh->SetDrawMode(GL::Model::eDM_Wireframe);

		line = new Renderable();
		line->modelPath = "LINE";
		line->shaderPath = "DebugDraw.shader";
		line->mpMesh = new GL::Model;
		line->mpMesh->AddVertex(GL::Vertex());
		line->mpMesh->AddVertex(GL::Vertex(glm::vec3(1.f, 1.f, 1.f)));
		line->mpMesh->SetPrimitiveType(GL::Model::eLineList);
		line->mpMesh->SetDrawMode(GL::Model::eDM_Wireframe);
		line->mpMesh->UploadToGPU();
		line->mpTexture = clcResources.GetTexture(line->texPath);
		line->mpShader = clcResources.GetShader(line->shaderPath);
	}

	void DebugSystem::Clear()
	{
		allObjects.clear();
	}

	void DebugSystem::DrawSphere(glm::vec3 pos, float radius, Color col)
	{
		DrawInfo obj;
		obj.trs.mPosition = pos;
		obj.trs.mScale = glm::vec3(radius * 2);

		obj.ren = sphere;

		obj.col = col;

		allObjects.push_back(obj);
	}

	void DebugSystem::DrawLine(glm::vec3 p0, glm::vec3 p1, Color col)
	{
		DrawInfo obj;
		obj.trs.mPosition = p0;
		obj.trs.mScale = p1 - p0;

		obj.ren = line;

		obj.col = col;

		allObjects.push_back(obj);
	}

	void DebugSystem::DrawCube(glm::vec3 pos, glm::vec3 sca, glm::vec3 rot, Color col)
	{
		DrawInfo obj;
		obj.trs.mPosition = pos;
		obj.trs.mScale = sca;
		obj.trs.mRotation = rot;

		obj.ren = cube;

		obj.col = col;

		allObjects.push_back(obj);
	}

	void DebugSystem::DrawCubeTwoCorners(glm::vec3 p0, glm::vec3 p1, Color col)
	{
		DrawInfo obj;
		obj.trs.mPosition = p0 + (p1 - p0) / 2.f;
		obj.trs.mScale = p1 - p0;

		obj.ren = cube;

		obj.col = col;

		allObjects.push_back(obj);
	}
}