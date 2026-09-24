#pragma once
#include <string>
#include <vector>
#include "../Engine/Singleton.h"
#include <glm/glm.hpp>
#include "../Graphics/Renderable.h"
#include "../Engine/GameObject.h"

#ifdef IN_LIBRARY
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif

namespace clc
{
	class API DebugSystem
	{
		CLC_SINGLETON(DebugSystem);

		struct DrawInfo
		{
			Transform trs;
			Renderable* ren;
			Color col;
		};

	public:
		void Initialize();
		void Clear();

		void DrawSphere(glm::vec3 pos, float radius, Color col);
		void DrawLine(glm::vec3 p0, glm::vec3 p1, Color col);
		void DrawCube(glm::vec3 pos, glm::vec3 sca, glm::vec3 rot, Color col);
		void DrawCubeTwoCorners(glm::vec3 p0, glm::vec3 p1, Color col);

		Renderable* sphere = nullptr;
		Renderable* cube = nullptr;
		Renderable* line = nullptr;

		std::vector<DrawInfo> allObjects;
	private:

	};
}

// Easy access to singleton
#define clcDebug (clc::DebugSystem::GetInstance())
