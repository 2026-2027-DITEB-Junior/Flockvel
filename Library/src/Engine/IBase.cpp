#include "pch.h"
#include "IBase.h"
#include "SceneManager.h"

namespace clc
{
	bool IBase::IsEnabled() const { return mEnabled; }
	void IBase::SetEnabled(bool enable) { mEnabled = enable; }
	void IBase::Shutdown() { clcSceneManager.SubscribeObject(this); }

	void IBase::Serialize(nlohmann::json& j) const
	{
		j["type"] = const_cast<IBase*>(this)->GetType();
		j["name"] = mName;
		j["enabled"] = mEnabled;
	}

	void IBase::Deserialize(const nlohmann::json& j)
	{
		if (j.contains("name"))
			mName = j["name"].get<std::string>();
		if (j.contains("enabled"))
			mEnabled = j["enabled"].get<bool>();
	}
}