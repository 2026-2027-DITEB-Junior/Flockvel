#include "pch.h"
#include "IComp.h"

namespace clc
{
	// default constructor (does nothing). 
	IComp::IComp() {}
	IComp::~IComp() {}  // destructor flags the class as pure virtual

	// basic state functions
	void IComp::AddToSystem() {}	     // called the object should added to its corresponding system
	void IComp::RemoveFromSystem() {}	 // called when the object is to be removed from its corresponding system.
	void IComp::OnCreate() {}			 // happens when the object's component is just created. 
	void IComp::Start() {}				 // happens just before the game/scene starts playing.
	void IComp::Shutdown()
	{
		if (mShutdown) return;
		RemoveFromSystem();
		IBase::Shutdown();
	}
	void IComp::Update() {}				 // default update of the object (might not be used)
	IComp* IComp::Clone() { return new IComp(*this); }
	void IComp::Draw() {}

	bool IComp::operator== (const IComp& comp) const { return true; }
	bool IComp::operator!= (const IComp& comp) const { return false; }

	GameObject* IComp::Owner() { return mOwner; }
}