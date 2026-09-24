#ifndef AEX_VEC3_H
#define AEX_VEC3_H
#include "AEXMathDLL.h"
#include "AEXVec2.h"

#pragma warning (disable:4201) // nameless struct warning

namespace aex
{
	struct AEX_MATH_API AEVec3
	{
		union
		{
			struct
			{
				float	x, y, z;
			};
			float		v[3];
		};

		AEVec3();
		AEVec3(const AEVec3& rhs);
		AEVec3(const AEVec2 &rhs2D, float zz = 0.f);
		AEVec3(float xx, float yy, float zz);
		explicit	AEVec3(float xx);
		operator	AEVec2()const;
		
		// add or substract										
		AEVec3		operator+		(const AEVec3& rhs) const;
		AEVec3&		operator+=		(const AEVec3& rhs);
		AEVec3		operator-		(const AEVec3& rhs) const;
		AEVec3&		operator-=		(const AEVec3& rhs);
		
		// multiply or divide by a scalar						
		AEVec3		operator*		(float s) const;
		AEVec3&		operator*=		(float s);
		AEVec3		operator/		(float s) const;
		AEVec3&		operator/=		(float s);

		// Vector length and distance			
		float			Length() const;
		float			LengthSq() const;
		float			Distance(const AEVec3& rhs) const;
		float			DistanceSq(const AEVec3 & rhs) const;
		AEVec3		Project(const AEVec3& onto) const;

		// calculate normalized vector							
		AEVec3		Normalize() const;
		const AEVec3&	NormalizeThis();
		
		// dot product											
		float			operator*		(const AEVec3& rhs) const;
		
		// cross product										
		AEVec3		Cross(const AEVec3& rhs) const;
	};

	// ---------------------------------------------------------------------------

	template <typename T>
	AEVec3 operator*(T s, const AEVec3& rhs)
	{
		return (rhs * s);
	}

	struct AEInteger4
	{
		union
		{
			struct
			{
				int x, y, z, w;
			};
			int		v[4];
		};
	};
}
#pragma warning (default:4201) // nameless struct warning

// ---------------------------------------------------------------------------
#endif