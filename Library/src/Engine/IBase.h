#pragma once
#include "../API.h"
#include "RTTI.h"
#include <nlohmann/json.hpp>	// nlohmann::json

//#include "AEXDataTypes.h"
//#include "Logic/EventSystem.h"

namespace clc
{
	/* ----------------------------------------------------------------------------
	*	This class provides the base for all the classes in the engine
	*	It provides basic services such as name, Rtti, enabled/disabled
	*	serialization, events, etc...
	*	Note that it derives from Rtti (using virtual inheritance)
	*/
	class API IBase : public virtual Rtti
	{
	friend class SceneManager;

	public:
		// constructor
		IBase() = default;
		// public virtual destructor 
		virtual ~IBase() = default;

		virtual bool IsEnabled() const;
		virtual void SetEnabled(bool enable);

		virtual void Shutdown(); // call this instead of destroying the object

		bool IsShutdown() const { return mShutdown; }

		std::string mName;

		virtual void Serialize(nlohmann::json& j) const;
		virtual void Deserialize(const nlohmann::json& j);

	protected:
		bool mEnabled = true; // by default enabled
		bool mShutdown = false; // determines if the object will be destroyed at the end of the frame
	};
}