namespace clc
{
	template <typename T>
	TResource<T>* ResourceManager::GetResource(const std::string& path)
	{
		std::string key = MakeKey(path);

		// already loaded
		auto it = mResources.find(key);
		if (it != mResources.end())
			return dynamic_cast<TResource<T>*>(it->second);

		if (path.empty()) return nullptr;
		TResource<T>* res = new TResource<T>;
		res->mPath = key;

		if (!res->Load()) {
			delete res;
			return nullptr;
		}

		mResources[key] = res;
		return res;
	}
}