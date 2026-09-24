#include "pch.h"
#include "Transform.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace clc;

glm::mat4 Transform::WorldMtx()
{
	return TranslationMtx() * RotMtx() * ScaleMtx();
}

glm::mat4 Transform::TranslationMtx()
{
	return glm::translate(glm::mat4(1.0f), { mPosition });
}

glm::mat4 Transform::ScaleMtx()
{
	return glm::scale(glm::mat4(1.0f), { mScale });
}

glm::mat4 Transform::RotMtx()
{
	return glm::rotate(glm::mat4(1.0f), mRotation.x, glm::vec3(1.f, 0.f, 0.f))
		* glm::rotate(glm::mat4(1.0f), mRotation.y, glm::vec3(0.f, 1.f, 0.f))
		* glm::rotate(glm::mat4(1.0f), mRotation.z, glm::vec3(0.f, 0.f, 1.f));
}

glm::mat4 Transform::InvWorldMtx()
{
	return InvScaleMtx() * InvRotMtx() * InvTranslationMtx();
}

glm::mat4 Transform::InvTranslationMtx()
{
	return glm::translate(glm::mat4(1.0f), { -mPosition });
}

glm::mat4 Transform::InvScaleMtx()
{
	return glm::scale(glm::mat4(1.0f), { 1.0f / mScale });
}

glm::mat4 Transform::InvRotMtx()
{
	return glm::rotate(glm::mat4(1.0f), -mRotation.z, glm::vec3(0.f, 0.f, 1.f))
		* glm::rotate(glm::mat4(1.0f), -mRotation.y, glm::vec3(0.f, 1.f, 0.f))
		* glm::rotate(glm::mat4(1.0f), -mRotation.x, glm::vec3(1.f, 0.f, 0.f));
}