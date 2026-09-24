namespace clc
{
	template <typename T> void GameObject::NewComp()
	{
		AddComp(new T);
	}

	template <typename T> T* GameObject::GetComp()
	{
		for (IComp* comp : mComps)
		{
			// dynamic cast so asking for a base type also finds the derived ones
			T* casted = dynamic_cast<T*>(comp);
			if (casted)
				return casted;
		}

		return nullptr;
	}

	template <typename T> std::vector<T*> GameObject::GetCompVec()
	{
		std::vector<T*> vec;

		for (IComp* comp : mComps)
		{
			T* casted = dynamic_cast<T*>(comp);
			if (casted)
				vec.push_back(casted);
		}

		return vec;
	}
}