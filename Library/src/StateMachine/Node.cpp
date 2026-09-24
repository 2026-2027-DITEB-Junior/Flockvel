#include "pch.h"
#include "Node.h"
namespace clc {
	Node::~Node() {}
	std::string Node::get_name()
	{
		return name_;
	}
	void Node::set_name(std::string name)
	{
		name_ = name;
	}
}