#ifndef AEX_VEC4_H_
#define AEX_VEC4_H_
#include "AEXMathDLL.h"
#include "AEXVec3.h"

#pragma warning (disable:4201) // nameless struct warning
namespace aex
{
	struct AEX_MATH_API AEVec4
	{
		union
		{
			struct
			{

				float x, y, z, w;
			};
			float v[4];
		};

		AEVec4(const AEVec4& rhs);
		AEVec4(const AEVec3& rhs3D, float ww = 0.f);
		AEVec4(float xx = 0.0f, float yy = 0.0f, float zz = 0.0f, float ww = 1.0f);
		AEVec3 vec3() const;

		AEVec4 operator *(const AEVec4 & rhs)const;
		AEVec4& operator *=(const AEVec4 & rhs);
		AEVec4 operator *(const float & sc)const;
		AEVec4& operator *=(const float & sc);
		AEVec4 operator +(const AEVec4 & rhs)const;
		AEVec4& operator +=(const AEVec4 & rhs);
		AEVec4 operator -(const AEVec4 & rhs)const;
		AEVec4& operator -=(const AEVec4 & rhs);
	};
}
#pragma warning (default:4201) // nameless struct warning


#endif