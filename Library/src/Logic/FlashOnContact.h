#pragma once
#include "../Logic/LogicSystem.h"
#include "../Engine/Transform.h"
#include "../Event System/EventSys.h"
#include "../Graphics/RenderModifier.h"
#include "../Graphics/GL/AEXColor.h"
#include <vector>
#include "../Audio/AudioSource.h"


namespace clc
{
	class Renderable;
	class FlashOnContact : public LogicComp, public subscriber, public RenderModifier
	{
	public:
		Renderable* ren = nullptr;

		void OnCreate() override;
		void Start() override;
		void Update() override;

		void handle_event(EEvent* e);
		void handle_event(EEvent& e);

		void PreRender() override;
		void PostRender() override;

		Color tintColor = Color(1.f, 1.f, 1.f, 1.f);
		float tintIntensity = 0.f;
		AudioSource* audio;
	};
}