#pragma once

#include "AEXGL_DLL.h"

#pragma warning (disable:4201) // nameless struct warning
namespace clc
{
	struct  API Color
	{
		union
		{
			struct
			{
				float r, g, b, a;
			};
			struct
			{

				float x, y, z, w;
			};
			float v[4];
		};
		Color(float rr = 0.0f, float gg = 0.0f, float bb = 0.0f, float aa = 1.0f);
		Color(unsigned int color); // custom constructor
		operator unsigned int()const;	// conversion operator

		Color operator *(const Color & rhs)const;
		Color& operator *=(const Color & rhs);
		Color operator *(const float & sc)const;
		Color& operator *=(const float & sc);
		Color operator +(const Color & rhs)const;
		Color& operator +=(const Color & rhs);
		Color operator -(const Color & rhs)const;
		Color& operator -=(const Color & rhs);
		Color operator!()const;
		Color Clamped()const;
	};
}
#pragma warning (default:4201) // nameless struct warning
