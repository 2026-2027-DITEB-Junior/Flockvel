#pragma once
#include "../API.h"
#include "Singleton.h"
#include "IBase.h"
#include <list>
#include <string>
#include <vector>
#include <cstdint>

namespace clc
{
    class GameObject;
    class API SceneManager
    {
        CLC_SINGLETON(SceneManager);
    public:
        bool Initialize();
        // Call between system updates. Invalid loads preserve the active scene.
        bool LoadScene(const std::string& filename);
        bool SaveScene(const std::string& filename);
        bool SaveScene();
        void UnloadScene();
        // Takes ownership of an initialized heap-allocated root.
        bool AddObject(GameObject* obj);
        const std::vector<GameObject*>& GetObjects() const { return mObjects; }
        std::vector<GameObject*> GetAllObjects() const;
        GameObject* GetObjectByName(const std::string& name) const;
        bool Contains(const GameObject* object) const;
        // Move a live object to another parent, or to the scene root (nullptr).
        // Its local transform and component registrations are preserved.
        bool ReparentObject(GameObject* object, GameObject* parent);
        const std::string& GetCurrentScenePath() const { return mCurrentScenePath; }
        const std::string& GetLastError() const { return mLastError; }
        std::uint64_t GetRevision() const { return mRevision; }
        void SubscribeObject(IBase* obj);
        void DeleteObjects();
    private:
        std::vector<GameObject*> mObjects;
        std::string mCurrentScenePath;
        std::string mLastError;
        std::list<IBase*> mShutdownObjects;
        std::uint64_t mRevision = 0;
    };
}
#define clcSceneManager (clc::SceneManager::GetInstance())
