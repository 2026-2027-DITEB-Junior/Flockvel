#include "pch.h"
#include "EventSys.h"
clc::EEvent::EEvent(){}

clc::EEvent::EEvent(std::string n) : name(n) {}

clc::PEvent::PEvent(std::string n, void* p){
	name = n;
	ptr = p;
}

clc::PEvent::~PEvent()
{
	ptr = nullptr;
}

void clc::subscriber::add_thrower(thrower* thr)
{
	if (std::find(throwers.begin(), throwers.end(), thr) == throwers.end())
		throwers.push_back(thr);

	if (std::find(thr->subs.begin(), thr->subs.end(), this) == thr->subs.end())
		thr->subs.push_back(this);
}

void clc::subscriber::remove_thrower(thrower* thr)
{
	if (std::find(thr->subs.begin(), thr->subs.end(), this) == thr->subs.end())
		thr->subs.erase(std::find(thr->subs.begin(), thr->subs.end(), this));

	if (std::find(throwers.begin(), throwers.end(), thr) == throwers.end())
		throwers.erase(std::find(throwers.begin(), throwers.end(), thr));
}

void clc::subscriber::handle_event(EEvent* e) {}
void clc::subscriber::handle_event(EEvent& e) {}

void clc::thrower::throw_event(const std::string& rhs)
{
	EEvent* e = new EEvent(rhs);
	for (subscriber* sub : subs) {
		sub->handle_event(e);
	}
}

void clc::thrower::throw_event(EEvent* e)
{
	for (subscriber* sub : subs) {
		sub->handle_event(e);
	}
}

void clc::thrower::throw_event(EEvent& e)
{
	for (subscriber* sub : subs) {
		sub->handle_event(e);
	}
}

clc::FlagEvent::FlagEvent(std::string n, int flag, glm::vec3 vec)
{
	name = n;
	flag_ = flag;
	vec_ = vec;
}

clc::FlagEvent::~FlagEvent()
{
}
