#pragma once

#include<vector>
#include<string>
#include <glm/glm.hpp>

#ifdef IN_LIBRARY
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif
namespace clc
{
	struct EEvent {
	public:
		EEvent();
		EEvent(std::string n);

		virtual ~EEvent(){}

		std::string name;
	};
	struct PEvent : public EEvent {
	public:
		PEvent(std::string n, void* p);

		~PEvent();
		void* ptr;
	};
	struct FlagEvent : public EEvent {
	public:
		FlagEvent(std::string n, int flag, glm::vec3 vec);

		~FlagEvent();
		int flag_;
		glm::vec3 vec_;
	};

	struct thrower;
	struct subscriber {

		void add_thrower(thrower* thr);
		void remove_thrower(thrower* thr);

		virtual void handle_event(EEvent* e);
		virtual void handle_event(EEvent& e);

	private:
		std::vector<thrower*> throwers;
	};

	struct thrower{
	public:
		virtual void throw_event(const std::string& rhs);
		virtual void throw_event(EEvent* e);
		virtual void throw_event(EEvent& e);

		std::vector<subscriber*> subs;
	};
}
#define clcEvent (clc::Event::GetInstance())