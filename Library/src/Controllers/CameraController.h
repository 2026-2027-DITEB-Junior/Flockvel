#pragma once
#include "../Graphics/Camera.h"
#include "../Logic/LogicSystem.h"
#include "../Engine/Transform.h"


namespace clc
{
	struct CameraController : public LogicComp
	{
	public:
		CameraController* Clone();

		Transform* transform = nullptr;
		Camera* cam = nullptr;
		GameObject* player = nullptr;

		void AddPlayer(GameObject* p);
		float camSpeed = 2.0f;
		float zoomSpeed = .30f;

		void OnCreate() override;
		void Start() override;
		void Update() override;

		bool operator== (const CameraController& cam);

		bool operator!= (const CameraController& cam);

		bool on_engine;
	};
}