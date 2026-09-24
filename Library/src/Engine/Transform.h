#pragma once
#include "../API.h"
#include "AEXMath.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace clc
{
	class API Transform
	{
	public:
		glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f };
		glm::vec3 mScale = { 1.0f, 1.0f, 1.0f };
		glm::vec3 mRotation = { 0.0f, 0.0f, 0.0f };

		glm::mat4 WorldMtx();
		glm::mat4 TranslationMtx();
		glm::mat4 ScaleMtx();
		glm::mat4 RotMtx();

		glm::mat4 InvWorldMtx();
		glm::mat4 InvTranslationMtx();
		glm::mat4 InvScaleMtx();
		glm::mat4 InvRotMtx();
	};
}