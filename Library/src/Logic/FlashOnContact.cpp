#include "pch.h"
#include "FlashOnContact.h"
#include "../Physics/PhysicsSystem.h"
#include "../Graphics/Renderable.h"
#include "../DeltaTime/DeltaTime.h"

namespace clc
{
	void FlashOnContact::OnCreate()
	{
		add_thrower(Owner()->GetComp<Rigidbody>());
		ren = Owner()->GetComp<Renderable>();
		ren->modifiers.push_back(this);
		audio = new AudioSource("parry-ultrakill.mp3");
		audio->mVolume = 0.5;
		audio->OnCreate();
		audio->Start();
	}

	void FlashOnContact::Start()
	{
		
	}

	void FlashOnContact::Update()
	{
		if (!enabled)
			return;

		tintIntensity -= clcDeltaTime.get_dt() * 4.3f;

		if (tintIntensity < 0.f)
		{
			tintIntensity = 0.f;
			enabled = false;
		}
	}

	void FlashOnContact::PreRender()
	{
		ren->mpShader->SetUniform("tintColor", tintColor);
		ren->mpShader->SetUniform("tintIntensity", tintIntensity);
	}

	void FlashOnContact::PostRender()
	{
		ren->mpShader->SetUniform("tintColor", Color(0.95f, 0.4f, 0.6f));
		ren->mpShader->SetUniform("tintIntensity", 0.f);
	}

	void FlashOnContact::handle_event(EEvent* e)
	{
		if (e->name == "Collision") {
			if (tintIntensity > 0.1f) return;
			tintIntensity = 1.f;
			enabled = true;

			audio->Play();
		}
	}

	void FlashOnContact::handle_event(EEvent& e)
	{
		handle_event(&e);
	}
}