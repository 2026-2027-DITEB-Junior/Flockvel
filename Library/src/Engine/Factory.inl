namespace clc
{
	template <typename T>
	void Factory::Register()
	{
		Register(Rtti::GetType<T>(), new TAllocator<T>);
	}

	template <typename T>
	T* Factory::Create()
	{
		return dynamic_cast<T*>(Create(Rtti::GetType<T>()));
	}
}