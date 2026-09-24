#include "pch.h"
#include "ResourceManager.h"
#include <GL/glew.h>
#include "../Graphics/GL/AEXTexture.h"
#include "../Graphics/GL/AEXShader.h"
#include "../Graphics/GL/AEXModel.h"
#include "../Audio/AudioSystem.h"
#include <filesystem>

namespace clc
{
	bool ResourceManager::Initialize()
	{
		// nothing to do here
		return true;
	}

	void ResourceManager::Shutdown()
	{
		Clear();
	}

	GL::Texture* ResourceManager::GetTexture(const std::string& path)
	{
		TResource<GL::Texture>* res = GetResource<GL::Texture>(ResourcePath(path, "textures"));
		if (!res)
			return nullptr;

		return res->Get();
	}

    std::vector<GL::Texture*> ResourceManager::GetCubeTexture(const std::string& path)
    {
        std::vector<GL::Texture*> textures;
        std::filesystem::path base = std::filesystem::u8path(path);
        for (size_t i = 0; i < mCubeFaces.size(); ++i)
        {
            std::string faceName = base.stem().u8string() + mCubeFaces[i] + base.extension().u8string();
            std::filesystem::path filename = base.parent_path() / faceName;
            GL::Texture* texture = GetTexture(filename.u8string());
            if (texture == nullptr)
            {
                textures.clear();
                return textures;
            }
            textures.push_back(texture);
        }
        return textures;
    }

	GL::ShaderProgram* ResourceManager::GetShader(const std::string& path)
	{
		TResource<GL::ShaderProgram>* res = GetResource<GL::ShaderProgram>(ResourcePath(path, "shaders"));
		if (!res)
			return nullptr;

		return res->Get();
	}

	GL::Model* ResourceManager::GetModel(const std::string& objFilePath)
	{
		TResource<GL::Model>* res = GetResource<GL::Model>(ResourcePath(objFilePath, "meshes"));
		if (!res)
			return nullptr;

		return res->Get();
	}

	FMOD::Sound* ResourceManager::GetSound(const std::string& path)
	{
		TResource<FMOD::Sound>* res = GetResource<FMOD::Sound>(ResourcePath(path, "sounds"));
		if (!res)
			return nullptr;

		return res->Get();
	}

	void ResourceManager::Remove(const std::string& path)
	{
		auto it = mResources.find(MakeKey(path));
		if (it == mResources.end())
			return;

		delete it->second;
		mResources.erase(it);
	}

	void ResourceManager::Clear()
	{
		for (auto& res : mResources) {
			delete res.second;
		}
		mResources.clear();
	}

	bool ResourceManager::LoadFolder(const std::string& path)
	{
		if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
			// find all files in current directory + SUBDIRECTORIES
            std::filesystem::path fullPath = std::filesystem::absolute(std::filesystem::u8path(path));
            for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(fullPath)) {
				if (std::filesystem::is_regular_file(entry.status())) {
					std::string ext = entry.path().extension().string();
					for (auto& c : ext) {
						c = (char)tolower(c);
					}

					// full path, subfolders included
					std::string resPath = entry.path().generic_string();

					if (ext == ".obj") {
						GetModel(resPath);
					}
					else if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
						GetTexture(resPath);
					}
					else if (ext == ".shader") {
						GetShader(resPath);
					}
					else if (ext == ".mp3" || ext == ".wav" || ext == ".ogg") {
						GetSound(resPath);
					}
				}
			}
			return true;
		}
		else {
			std::cout << "load folder: invalid path" << std::endl;
			return false;
		}
	}
    void ResourceManager::SetAssetRoot(const std::string& path)
    {
        std::filesystem::path root = std::filesystem::absolute(std::filesystem::u8path(path));
        mAssetRoot = root.lexically_normal().u8string();
    }

    std::string ResourceManager::ResolvePath(const std::string& path) const
    {
        std::filesystem::path resolved = std::filesystem::u8path(path).lexically_normal();
        if (resolved.is_relative() && !mAssetRoot.empty())
        {
            // Ambas formas se refieren a la carpeta data del proyecto.
            std::string relative = resolved.generic_u8string();
            if (relative.find("../data/") == 0)
                resolved = std::filesystem::u8path(relative.substr(3));

            resolved = std::filesystem::u8path(mAssetRoot) / resolved;
        }
        return resolved.lexically_normal().u8string();
    }

    std::string ResourceManager::ResourcePath(const std::string& path, const char* folder) const
    {
        if (path.empty())
            return std::string();

        std::filesystem::path file = std::filesystem::u8path(path);
        std::string relative = file.generic_u8string();
        bool includesDataFolder = relative.find("data/") == 0 || relative.find("../data/") == 0;
        if (file.is_absolute() || includesDataFolder)
            return ResolvePath(path);

        // Un nombre como cube.obj necesita la carpeta del tipo de recurso.
        std::filesystem::path fullPath = std::filesystem::path("data") / folder / file;
        return ResolvePath(fullPath.u8string());
    }

    std::string ResourceManager::MakeKey(const std::string& path) const
    {
        std::error_code error;
        std::filesystem::path resolved = std::filesystem::u8path(ResolvePath(path));
        std::filesystem::path canonical = std::filesystem::weakly_canonical(resolved, error);
        if (error)
            return resolved.u8string();
        return canonical.u8string();
    }

	// textures

	template <> bool TResource<GL::Texture>::Load()
	{
		if (!std::filesystem::is_regular_file(std::filesystem::u8path(mPath))) return false;
		mData = new GL::Texture(mPath.c_str());

		// no size -> the image wasnt found
		return mData->GetWidth() != 0;
	}

	template <> void TResource<GL::Texture>::Unload()
	{
		delete mData;
		mData = nullptr;
	}

	// shaders

	template <> bool TResource<GL::ShaderProgram>::Load()
	{
		// .shader file with the vertex and fragment paths
		if (!std::filesystem::is_regular_file(std::filesystem::u8path(mPath))) return false;
		mData = new GL::ShaderProgram(mPath.c_str());

		// check if it linked
		GLint status = GL_FALSE;
		glGetProgramiv(mData->GetOpenGLHandle(), GL_LINK_STATUS, &status);

		return status == GL_TRUE;
	}

	template <> void TResource<GL::ShaderProgram>::Unload()
	{
		delete mData;
		mData = nullptr;
	}

	// models

	template <> bool TResource<GL::Model>::Load()
	{
		if (!std::filesystem::is_regular_file(std::filesystem::u8path(mPath))) return false;
		mData = new GL::Model();
		mData->LoadFromFile(mPath.c_str());

		// no vertices -> the obj wasnt loaded
		return mData->GetVertexCount() != 0;
	}

	template <> void TResource<GL::Model>::Unload()
	{
		delete mData;
		mData = nullptr;
	}

	// sounds

	template <> bool TResource<FMOD::Sound>::Load()
	{
		mData = clcAudio.LoadSound(mPath);

		// null -> fmod could not open the file
		return mData != nullptr;
	}

	template <> void TResource<FMOD::Sound>::Unload()
	{
		// fmod owns the sound
		if (mData)
			mData->release();

		mData = nullptr;
	}
}