#ifndef AE_VEC2_H
#define AE_VEC2_H

#include "AEXMathDLL.h"

// ---------------------------------------------------------------------------

#pragma warning (disable:4201) // nameless struct warning
namespace aex
{
	struct AEX_MATH_API AEVec2
	{
		union
		{
			struct
			{
				float x, y;
			};
			float v[2];
		};

		// ------------------------------------------------------------------------
		AEVec2();
		AEVec2(const AEVec2& rhs);
		AEVec2(const float xx, const float yy);
		explicit	AEVec2(const float xx);

		// add or substract
		AEVec2		operator+	(const AEVec2& rhs) const;
		AEVec2		operator+=	(const AEVec2& rhs);
		AEVec2		operator-	(const AEVec2& rhs) const;
		AEVec2		operator-=	(const AEVec2& rhs);

		// multiply or divide by a scalar
		AEVec2		operator*	(float s) const;
		AEVec2		operator*=	(float s);
		AEVec2		operator/	(float s) const;
		AEVec2		operator/=	(float s);

		// unitary negation
		AEVec2		operator-() const;

		bool operator==(AEVec2 rhs) const;

		// perpendicualr
		AEVec2		Perp() const;

		// vector length
		float			Length() const;
		float			LengthSq() const;

		// Distance
		float			Distance(const AEVec2 &rhs)const;
		float			DistanceSq(const AEVec2 &rhs)const;

		// Normalize
		AEVec2		Normalize() const;
		const AEVec2 &NormalizeThis();

		// dot product
		float			Dot(const AEVec2& rhs) const;
		float			operator*		(const AEVec2& rhs) const;

		//Just multiply both floats
		AEVec2		multiply	(const AEVec2& rhs) const;

		// projection
		AEVec2		Project(const AEVec2& rhs)const;
		AEVec2		ProjectPerp(const AEVec2& rhs)const;

		// cross product magnitude
		float			CrossMag(const AEVec2& rhs)const;

		// To and From Angle
		float			GetAngle()const;
		void		FromAngle(float angle);

		// Random
		static AEVec2 Random(AEVec2 min = AEVec2(0, 0), AEVec2 max = AEVec2(1, 1));
		static AEVec2 Lerp(AEVec2 from, AEVec2 to, float tn);

		static AEVec2 abs(const AEVec2& pt)
		{
			AEVec2 pos = pt;

			if (pos.x < 0)
				pos.x *= -1;
			if (pos.y < 0)
				pos.y *= -1;

			return pos;
		}

	};
}


// ---------------------------------------------------------------------------

#endif // VEC2_H