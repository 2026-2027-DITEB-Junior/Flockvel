#include "pch.h"
#include "Factory.h"
#include "GameObject.h"
#include "../Controllers/PlayerController.h"
#include "../Graphics/Camera.h"
#include "../Graphics/Renderable.h"
#include "../Audio/AudioSource.h"
#include "../Physics/Dynamics/BoxRb.h"
#include "../Physics/Dynamics/SphereRb.h"

namespace clc
{
	bool Factory::Initialize()
	{
		Register<GameObject>();
		Register<PlayerController>();
		Register<Camera>();
		Register<Renderable>();
		Register<AudioSource>();
		Register<BoxRb>();
		Register<SphereRb>();

		return true;
	}


	IBase* Factory::Create(const std::string& type)
	{
		auto it = mAllocators.find(type);
		if (it == mAllocators.end())
		{
			std::cout << "unknown type: " << type << "\n";
			return nullptr;
		}

		return it->second->Create();
	}

	void Factory::Register(const std::string& type, Allocator* allocator)
	{
		if (!mAllocators.count(type))
			mAllocators[type] = allocator;
		else
			delete allocator;
	}

	std::vector<std::string> Factory::getAllocatedComponents()
	{
		std::vector<std::string> componentStr;
		for (auto comps : mAllocators)
		{
			componentStr.push_back(comps.first);
		}
		return componentStr;
	}

	IBase* Factory::CreateFromJson(const json& j, const std::string& defaultType) {
		if (!j.is_object()) return nullptr;

		std::string type = defaultType;
		if (j.contains("type")) type = j["type"].get<std::string>();

		if (type.empty()) return nullptr;

		// create already checks the map and complains if the type is not there
		IBase* obj = clcFactory.Create(type);
		if (!obj) return nullptr;

		try
		{
			obj->Deserialize(j);
		}
		catch (...)
		{
			// Deserialization may already have allocated children and registered components.
			obj->Shutdown();
			throw;
		}
		return obj;
	}
}