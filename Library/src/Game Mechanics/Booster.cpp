#include "pch.h"
#include "Booster.h"
#include "../Physics/PhysicsSystem.h"
#include "../Graphics/Renderable.h"
#include "../DeltaTime/DeltaTime.h"

namespace clc
{
	void Booster::OnCreate()
	{
		add_thrower(Owner()->GetComp<Rigidbody>());
		ren = Owner()->GetComp<Renderable>();
		ren->modifiers.push_back(this);
	}

	void Booster::Start()
	{
		
	}

	void Booster::Update()
	{
		if (!enabled)
			return;

		timer -= clcDeltaTime.get_dt();

		if (timer < 0.f)
		{
			enabled = false;
		}
		else
		{
			pc->rb->AddForce(glm::vec3(1000.f, 0.f, 0.f));
		}
		
	}

	void Booster::PreRender()
	{

	}

	void Booster::PostRender()
	{

	}

	void Booster::handle_event(EEvent* e)
	{
		if (e->name == "Trigger") {
			if (timer >= 0.01f)
				return;

			timer = 0.2f;
			enabled = true;
		}
	}

	void Booster::handle_event(EEvent& e)
	{
		handle_event(&e);
	}
}