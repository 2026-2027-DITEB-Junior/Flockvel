#include "pch.h"
#include "Factory.h"
#include "GameObject.h"
#include "Serialization.h"
#include <stdexcept>

using namespace clc;
namespace clc
{
#pragma region COMPONENT_ACTIVATION
	void GameObject::SetEnabled(bool enable)
	{
		mEnabled = enable;

		for (IComp* comp : mComps)
			comp->SetEnabled(enable);

		for (GameObject* child : mChildren)
			child->SetEnabled(enable);
	}
	
    bool GameObject::IsActiveInHierarchy() const
    {
        if (IsShutdown() || !IsEnabled())
            return false;
        if (mParent != nullptr)
            return mParent->IsActiveInHierarchy();
        return true;
    }
    glm::mat4 GameObject::GetWorldMatrix() const
    {
        Transform local = mTransform;
        glm::mat4 worldMatrix = local.WorldMtx();
        if (mParent != nullptr)
            worldMatrix = mParent->GetWorldMatrix() * worldMatrix;
        return worldMatrix;
    }

	void GameObject::OnCreate()
	{
		for (IComp* comp : mComps)
			comp->OnCreate();

		for (GameObject* child : mChildren)
			child->OnCreate();
	}

	void GameObject::Start()
	{
		for (IComp* comp : mComps)
			comp->Start();

		for (GameObject* child : mChildren)
			child->Start();
	}

	void GameObject::AddToSystem()
	{
		for (IComp* comp : mComps)
			comp->AddToSystem();

		for (GameObject* child : mChildren)
			child->AddToSystem();
	}

	void GameObject::RemoveFromSystem()
	{
		for (IComp* comp : mComps)
			comp->RemoveFromSystem();

		for (GameObject* child : mChildren)
			child->RemoveFromSystem();
	}

	void GameObject::Shutdown()
	{
		if (mShutdown) return;
		DeleteAllComps();
		DeleteAllChildren();
		IBase::Shutdown();
	}
#pragma endregion
#pragma region SERIALIZATION
	void GameObject::Serialize(nlohmann::json& j) const {
		IBase::Serialize(j);

		j["transform"] = mTransform;

		// components and children stored as arrays of objects
		j["components"] = nlohmann::json::array();
		nlohmann::json& jComps = j["components"];
		for (const IComp* comp : mComps) {
			if (comp->IsShutdown()) continue;
			nlohmann::json jComp;
			comp->Serialize(jComp);
			jComps.push_back(jComp);
		}

		j["children"] = nlohmann::json::array();
		nlohmann::json& jChildren = j["children"];
		for (const GameObject* child : mChildren) {
			if (child->IsShutdown()) continue;
			nlohmann::json jChild;
			child->Serialize(jChild);
			jChildren.push_back(jChild);
		}
	}

	void GameObject::Deserialize(const nlohmann::json& j) {
		if (!j.is_object() ||
			(j.contains("components") && !j["components"].is_array()) ||
			(j.contains("children") && !j["children"].is_array()))
			throw std::runtime_error("invalid GameObject, components or children");

		IBase::Deserialize(j);

		// clearing what was here before so loading twice does not duplicate
		DeleteAllComps();
		DeleteAllChildren();

		if (j.contains("transform"))
			mTransform = j["transform"].get<Transform>();

		if (j.contains("components") && j["components"].is_array()) {
			for (const nlohmann::json& jComp : j["components"]) {
				// CreateFromJson allocates through the factory and deserializes
				IBase* obj = clcFactory.CreateFromJson(jComp);
				IComp* comp = dynamic_cast<IComp*>(obj);

				if (!comp) {
					if (obj) obj->Shutdown();
					throw std::runtime_error("component is not a registered IComp");
				}

				// addcomp sets the owner
				AddComp(comp);
				// SceneManager initializes the complete graph before registering it.
			}
		}

		if (j.contains("children") && j["children"].is_array()) {
			for (const nlohmann::json& jChild : j["children"]) {
				// children default to go if they have no type
				IBase* obj = clcFactory.CreateFromJson(jChild, Rtti::GetType<GameObject>());
				GameObject* child = dynamic_cast<GameObject*>(obj);

				if (!child) {
					if (obj) obj->Shutdown();
					throw std::runtime_error("child is not a registered GameObject");
				}

				// addchild sets the parent
				AddChild(child);
			}
		}
	}
#pragma endregion
#pragma region CHILDREN_COMPONENT_GESTION
	void GameObject::AddComp(IComp* ic)
	{
		for (IComp* comp : mComps)
		{
			if (comp == ic)
				return;
		}

		ic->mOwner = this;
		mComps.push_back(ic);
	}
	void GameObject::DeleteComp(IComp* ic)
	{
		for (auto i = mComps.begin(); i < mComps.end(); i++)
		{
			if (*i != ic)
				continue;

			(*i)->Shutdown();
			(*i)->mOwner = nullptr;
			mComps.erase(i);
			break;
		}
	}
	void GameObject::DeleteAllComps()
	{
		for (IComp* comp : mComps)
		{
			comp->Shutdown();
			comp->mOwner = nullptr;
		}
		mComps.clear();
	}

	void GameObject::AddChild(GameObject* obj)
	{
		for (GameObject* child : mChildren)
		{
			if (child == obj)
				return;
		}

		obj->mParent = this;
		mChildren.push_back(obj);
	}
	void GameObject::RemoveChild(GameObject* obj)
	{
		for (auto i = mChildren.begin(); i < mChildren.end(); i++)
		{
			if (*i != obj)
				continue;

			(*i)->Shutdown();
			(*i)->mParent = nullptr;
			mChildren.erase(i);
			break;
		}
	}
	void GameObject::RemoveAllChildren()
	{
		for (GameObject* child : mChildren)
			child->mParent = mParent;

		mChildren.clear();
	}
	void GameObject::DeleteAllChildren()
	{
		for (GameObject* child : mChildren)
		{
			child->Shutdown();
			child->mParent = nullptr;
		}

		mChildren.clear();
	}
#pragma endregion
}