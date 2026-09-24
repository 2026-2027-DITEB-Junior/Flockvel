#pragma once

namespace clc
{
	class RenderModifier{
	public:
		bool enabled = false;
		virtual ~RenderModifier();
		virtual void PreRender();
		virtual void PostRender();
	};
}