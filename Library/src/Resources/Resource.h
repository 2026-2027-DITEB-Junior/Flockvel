#pragma once
#include <string>
#include "../API.h"

namespace FMOD {
	class Sound;
}

namespace clc
{
	namespace GL
	{
		class Texture;
		class ShaderProgram;
		class Model;
	}

	// base class for every resource
	class API IResource
	{
	public:
		virtual ~IResource() = default;

		virtual bool Load() = 0;
		virtual void Unload() = 0;

		// file it comes from, also the key in the manager
		std::string mPath;
	};

	// holds the data of one resource of type T
	template <typename T> class TResource : public IResource
	{
	public:
		~TResource() { Unload(); }

		bool Load() override;
		void Unload() override;

		T* Get() { return mData; }

	private:
		T* mData = nullptr;
	};

	// every type needs its own load and unload
	template <> bool TResource<GL::Texture>::Load();
	template <> void TResource<GL::Texture>::Unload();

	template <> bool TResource<GL::ShaderProgram>::Load();
	template <> void TResource<GL::ShaderProgram>::Unload();

	template <> bool TResource<GL::Model>::Load();
	template <> void TResource<GL::Model>::Unload();

	template <> bool TResource<FMOD::Sound>::Load();
	template <> void TResource<FMOD::Sound>::Unload();
}