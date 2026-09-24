#pragma once
#include "Resource.h"
#include "../CommonHeaders.h"
#include "../Engine/Singleton.h"
#include "../API.h"

namespace clc
{
	// one copy of every loaded file
	class API ResourceManager
	{
		CLC_SINGLETON(ResourceManager);

	public:
		// init and cleanup functions
		bool Initialize();
		void SetAssetRoot(const std::string& path);
		const std::string& GetAssetRoot() const { return mAssetRoot; }
		std::string ResolvePath(const std::string& path) const;
		void Shutdown();

		// loads the file the first time only
		template <typename T> TResource<T>* GetResource(const std::string& path);

		// ready to use
		GL::Texture* GetTexture(const std::string& path);
		std::vector<GL::Texture*> GetCubeTexture(const std::string& path);
		GL::ShaderProgram* GetShader(const std::string& path);
		GL::Model* GetModel(const std::string& objFilePath);
		FMOD::Sound* GetSound(const std::string& path);

		// frees one resource
		void Remove(const std::string& path);

		// frees all of them
		void Clear();

		// convenience function that loads all resources in a folder (warmup)
		bool LoadFolder(const std::string& path);

	private:
		// same file written different ways -> same key
		std::string MakeKey(const std::string& path) const;
        std::string ResourcePath(const std::string& path, const char* folder) const;

		std::map<std::string, IResource*> mResources;

		std::vector<std::string> mCubeFaces = {"right", "left", "top", "bottom", "front", "back" };
		std::string mAssetRoot;
	};
}

#include "ResourceManager.inl"

#define clcResources (clc::ResourceManager::GetInstance())