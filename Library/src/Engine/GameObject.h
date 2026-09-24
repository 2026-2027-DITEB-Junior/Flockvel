#pragma once
#include "../API.h"
#include "../CommonHeaders.h"
#include "IComp.h"
#include "Transform.h"
#include "AEXMath.h"

namespace clc
{
	class IComp;
	class API GameObject : public IBase
	{
		friend class SceneManager;
	public:
		// constructor - does nothing
		GameObject() = default;

		Transform mTransform;

		// read and write the whole transform
		const Transform& GetTransform() const { return mTransform; }
		void SetTransform(const Transform& t) { mTransform = t; }

		void SetEnabled(bool enable) override; // inherited from IBase
		void OnCreate();
		void Start();
		void AddToSystem();
		void RemoveFromSystem();
		void Shutdown() override;

		void Serialize(nlohmann::json& j) const override;
		void Deserialize(const nlohmann::json& j) override;

		void AddComp(IComp* ic);
		void DeleteComp(IComp* ic);
		void DeleteAllComps();

		GameObject* GetParent() const { return mParent; }
		bool IsActiveInHierarchy() const;
		glm::mat4 GetWorldMatrix() const;

		const std::vector<GameObject*>& GetChildren() const { return mChildren; }

		void AddChild(GameObject* obj);
		void RemoveChild(GameObject* obj);
		void RemoveAllChildren();
		void DeleteAllChildren();

		// Use this when there is a single instance of the component type
		template <typename T> T* GetComp();
		// Use this when there are multiple instances of the component type or you are unsure
		template <typename T> std::vector<T*> GetCompVec();
		template <typename T> void NewComp();

		glm::vec3 mForward{ 1.0f, 0.0f, 0.0f };
		glm::vec3 mUp{ 0.0f, 1.0f, 0.0f };
	private:

		std::vector<IComp*> mComps;

		GameObject* mParent = nullptr;
		std::vector<GameObject*> mChildren;
	};
}

#include "GameObject.inl"