#include "pch.h"
#include "SceneManager.h"
#include "Factory.h"
#include "GameObject.h"
#include "Serialization.h"
#include <stdexcept>

namespace clc
{
    namespace
    {
        void CollectObjects(GameObject* object, std::vector<GameObject*>& objects)
        {
            if (object == nullptr || object->IsShutdown())
                return;

            objects.push_back(object);
            const std::vector<GameObject*>& children = object->GetChildren();
            for (size_t i = 0; i < children.size(); ++i)
                CollectObjects(children[i], objects);
        }

        void ShutdownObjects(const std::vector<GameObject*>& objects)
        {
            for (size_t i = 0; i < objects.size(); ++i)
                objects[i]->Shutdown();
        }

        // Quita el puntero de la lista; no destruye el objeto.
        void RemoveObjectFromList(std::vector<GameObject*>& objects, GameObject* object)
        {
            size_t i = 0;
            while (i < objects.size())
            {
                if (objects[i] == object)
                    objects.erase(objects.begin() + i);
                else
                    ++i;
            }
        }
    }

    bool SceneManager::Initialize()
    {
        return true;
    }

    bool SceneManager::LoadScene(const std::string& filename)
    {
        // Copiamos la ruta: filename puede ser la propia mCurrentScenePath.
        const std::string path = filename;
        json scene;
        std::vector<GameObject*> newObjects;
        mLastError.clear();

        // 1. Leer y construir la escena nueva sin borrar la actual.
        try
        {
            if (!ReadJsonFile(path, scene))
            {
                mLastError = "Cannot read the scene or invalid JSON: " + path;
                return false;
            }

            json objects;
            if (scene.is_object() && scene.contains("objects") && scene["objects"].is_array())
            {
                objects = scene["objects"];
            }
            else if (scene.is_object() && !scene.contains("objects") && scene.contains("type"))
            {
                // Tambien aceptamos un archivo que contiene un solo GameObject.
                objects = json::array();
                objects.push_back(scene);
            }
            else
            {
                mLastError = "Expected an objects array or a serialized GameObject.";
                return false;
            }

            newObjects.reserve(objects.size());
            for (size_t i = 0; i < objects.size(); ++i)
            {
                IBase* created = clcFactory.CreateFromJson(objects[i], Rtti::GetType<GameObject>());
                GameObject* object = dynamic_cast<GameObject*>(created);
                if (object == nullptr)
                {
                    if (created != nullptr)
                        created->Shutdown();

                    throw std::runtime_error("Scene root is not a registered GameObject.");
                }
                newObjects.push_back(object);
            }

            // Todos los componentes ya tienen su propietario antes de inicializarse.
            for (size_t i = 0; i < newObjects.size(); ++i)
                newObjects[i]->OnCreate();
        }
        catch (const std::exception& error)
        {
            ShutdownObjects(newObjects);
            mLastError = error.what();
            return false;
        }

        // 2. Activar la nueva escena. swap intercambia listas, no copia objetos.
        const std::string previousPath = mCurrentScenePath;
        std::vector<GameObject*> previousObjects;
        previousObjects.swap(mObjects);
        mObjects.swap(newObjects);
        mCurrentScenePath = path;

        try
        {
            for (size_t i = 0; i < mObjects.size(); ++i)
                mObjects[i]->AddToSystem();

            for (size_t i = 0; i < mObjects.size(); ++i)
                mObjects[i]->Start();
        }
        catch (const std::exception& error)
        {
            // Si falla el inicio, retiramos lo nuevo y recuperamos la escena anterior.
            ShutdownObjects(mObjects);
            mObjects.swap(previousObjects);
            mCurrentScenePath = previousPath;
            mLastError = error.what();
            return false;
        }

        // 3. La carga ha terminado: ya podemos retirar la escena anterior.
        ShutdownObjects(previousObjects);
        ++mRevision;
        return true;
    }

    bool SceneManager::SaveScene(const std::string& filename)
    {
        const std::string path = filename;
        mLastError.clear();
        if (path.empty())
        {
            mLastError = "Choose a scene filename first.";
            return false;
        }

        try
        {
            json scene;
            scene["objects"] = json::array();
            for (size_t i = 0; i < mObjects.size(); ++i)
            {
                GameObject* object = mObjects[i];
                if (object->IsShutdown() || object->GetParent() != nullptr)
                    continue;

                json savedObject;
                object->Serialize(savedObject);
                scene["objects"].push_back(savedObject);
            }

            if (!WriteJsonFile(path, scene))
            {
                mLastError = "Cannot save scene: " + path;
                return false;
            }

            mCurrentScenePath = path;
            return true;
        }
        catch (const std::exception& error)
        {
            mLastError = error.what();
            return false;
        }
    }

    bool SceneManager::SaveScene()
    {
        return SaveScene(mCurrentScenePath);
    }

    void SceneManager::UnloadScene()
    {
        ShutdownObjects(mObjects);
        mObjects.clear();
        mCurrentScenePath.clear();
        mLastError.clear();
        ++mRevision;
    }

    bool SceneManager::AddObject(GameObject* object)
    {
        if (object == nullptr || object->IsShutdown() || object->GetParent() != nullptr)
            return false;

        for (size_t i = 0; i < mObjects.size(); ++i)
        {
            if (mObjects[i] == object)
                return true;
        }

        mObjects.push_back(object);
        return true;
    }

    std::vector<GameObject*> SceneManager::GetAllObjects() const
    {
        std::vector<GameObject*> result;
        for (size_t i = 0; i < mObjects.size(); ++i)
            CollectObjects(mObjects[i], result);

        return result;
    }

    GameObject* SceneManager::GetObjectByName(const std::string& name) const
    {
        const std::vector<GameObject*> objects = GetAllObjects();
        for (size_t i = 0; i < objects.size(); ++i)
        {
            if (objects[i]->mName == name)
                return objects[i];
        }
        return nullptr;
    }

    bool SceneManager::Contains(const GameObject* object) const
    {
        const std::vector<GameObject*> objects = GetAllObjects();
        for (size_t i = 0; i < objects.size(); ++i)
        {
            if (objects[i] == object)
                return true;
        }
        return false;
    }

    bool SceneManager::ReparentObject(GameObject* object, GameObject* parent)
    {
        mLastError.clear();
        if (object == nullptr || !Contains(object) || (parent != nullptr && !Contains(parent)))
        {
            mLastError = "The object and its parent must belong to the active scene.";
            return false;
        }

        // Subir por los padres permite comprobar si el cambio crearia un ciclo.
        GameObject* ancestor = parent;
        while (ancestor != nullptr)
        {
            if (ancestor == object)
            {
                mLastError = "An object cannot be parented to itself or one of its descendants.";
                return false;
            }
            ancestor = ancestor->mParent;
        }

        if (object->mParent == parent)
            return true;

        std::vector<GameObject*>* oldList = &mObjects;
        if (object->mParent != nullptr)
            oldList = &object->mParent->mChildren;

        std::vector<GameObject*>* newList = &mObjects;
        if (parent != nullptr)
            newList = &parent->mChildren;

        // Reservar antes de quitarlo evita perder el objeto si falta memoria.
        newList->reserve(newList->size() + 1);
        RemoveObjectFromList(*oldList, object);
        newList->push_back(object);
        object->mParent = parent;
        return true;
    }

    void SceneManager::SubscribeObject(IBase* object)
    {
        if (object == nullptr || object->mShutdown)
            return;

        // GameObject encola sus componentes e hijos antes que a si mismo.
        mShutdownObjects.push_back(object);
        object->mShutdown = true;
    }

    void SceneManager::DeleteObjects()
    {
        while (!mShutdownObjects.empty())
        {
            IBase* object = mShutdownObjects.front();
            mShutdownObjects.pop_front();

            GameObject* gameObject = dynamic_cast<GameObject*>(object);
            if (gameObject != nullptr)
            {
                RemoveObjectFromList(mObjects, gameObject);
                if (gameObject->mParent != nullptr)
                    RemoveObjectFromList(gameObject->mParent->mChildren, gameObject);
            }
            else
            {
                IComp* component = dynamic_cast<IComp*>(object);
                if (component != nullptr && component->mOwner != nullptr)
                {
                    std::vector<IComp*>& components = component->mOwner->mComps;
                    size_t i = 0;
                    while (i < components.size())
                    {
                        if (components[i] == component)
                            components.erase(components.begin() + i);
                        else
                            ++i;
                    }
                }
            }
            delete object;
        }
    }
}
