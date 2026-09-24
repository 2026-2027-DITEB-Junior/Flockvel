#include "pch.h"
#include "CameraController.h"

namespace clc
{
	void CameraController::OnCreate() {
		cam = Owner()->GetComp<Camera>();
		transform = &Owner()->mTransform;
	}

	void CameraController::AddPlayer(GameObject* p)
	{
		player = p;
	}

	void CameraController::Start() {
		// try to find the player
		/*for (auto obj : objects)
			if (obj->GetName() == "Player")player = obj;*/
		on_engine = true;
	}

	void CameraController::Update() {
		if (!(transform && cam))return;

		cam->target = player->mTransform.mPosition;
	}

	CameraController* CameraController::Clone() { return new CameraController(*this); }

	bool CameraController::operator== (const CameraController& cam)
	{
		if (camSpeed - cam.camSpeed > EPSILON2 || camSpeed - cam.camSpeed < -EPSILON2)
			return false;

		if (zoomSpeed - cam.zoomSpeed > EPSILON2 || zoomSpeed - cam.zoomSpeed < -EPSILON2)
			return false;

		return true;
	}

	bool CameraController::operator!= (const CameraController& cam)
	{
		return !(*this == cam);
	}
}