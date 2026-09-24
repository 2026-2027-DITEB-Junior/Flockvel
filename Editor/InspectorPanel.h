#pragma once
#include "Engine/GameObject.h"


class TransformPanel
{
public:
	void Draw(clc::Transform& transform, bool& dirty);
};

class InspectorPanel
{
public:
	void Draw(clc::GameObject* selected, bool& visible, bool& dirty);

private:

	TransformPanel m_transformPanel;
};
