#pragma once
#include "../API.h"
#include "IBase.h"
#include "GameObject.h"

namespace clc
{
	class API IComp : public IBase
	{
	friend class GameObject;
	friend class SceneManager;
		
	public:
		// default constructor (does nothing). 
		IComp();
		virtual ~IComp();  // destructor flags the class as pure virtual

		// basic state functions
		void Shutdown() override;
		virtual void AddToSystem();	     // called the object should added to its corresponding system
		virtual void RemoveFromSystem(); // called when the object is to be removed from its corresponding system.
		virtual void OnCreate();		 // happens when the object's component is just created. 
		virtual void Start();			 // happens just before the game/scene starts playing.
		virtual void Update();		     // default update of the object (might not be used)
		virtual IComp* Clone();
		virtual void Draw();

		virtual bool operator== (const IComp& comp) const;
		virtual bool operator!= (const IComp& comp) const;

		GameObject* Owner();

		bool shutdown = false; // determines if an object will be deleted at the end of the frame

	protected:
		GameObject* mOwner = nullptr; // note: GameObject should always be set to a value
	};
}