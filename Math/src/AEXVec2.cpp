// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEVec.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	implementation of the 2D vector library
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#include "AEXMath.h"

namespace aex
{
	AEVec2::AEVec2()
		: x(0.0f)
		, y(0.0f)
	{}
	AEVec2::AEVec2(const AEVec2& rhs)
		: x(rhs.x)
		, y(rhs.y)
	{}
	AEVec2::AEVec2(const float xx, const float yy)
		: x(xx)
		, y(yy)
	{}
	AEVec2::AEVec2(const float xx)
		: x(xx)
		, y(xx)
	{}

	// add or substract
	AEVec2		AEVec2::operator+	(const AEVec2& rhs) const
	{
		return AEVec2(x + rhs.x, y + rhs.y);
	}
	AEVec2		AEVec2::operator+=	(const AEVec2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return (*this);
	}
	AEVec2		AEVec2::operator-	(const AEVec2& rhs) const
	{
		return AEVec2(x - rhs.x, y - rhs.y);
	}
	AEVec2		AEVec2::operator-=	(const AEVec2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return (*this);
	}

	// multiply or divide by a scalar
	AEVec2		AEVec2::operator*	(float s) const
	{
		return AEVec2(x * s, y * s);
	}
	AEVec2		AEVec2::operator/	(float s) const
	{
		return AEVec2(x / s, y / s);
	}
	AEVec2		AEVec2::operator*=	(float s)
	{
		x *= s;
		y *= s;
		return (*this);
	}
	AEVec2		AEVec2::operator/=	(float s)
	{
		x /= s;
		y /= s;
		return (*this);
	}

	AEVec2		AEVec2::operator-() const
	{
		return AEVec2(-x, -y);
	}

	bool AEVec2::operator==(AEVec2 rhs) const
	{
		return fabs(x - rhs.x) < EPSILON2 && fabs(y - rhs.y) < EPSILON2;
	}

	// vector length
	float			AEVec2::Length() const
	{
		return sqrtf(x * x + y * y);
	}
	float			AEVec2::LengthSq() const
	{
		return x*x + y*y;
	}

	// Distance
	float			AEVec2::Distance(const AEVec2 &rhs)const
	{
		return (rhs - *this).Length();
	}
	float			AEVec2::DistanceSq(const AEVec2 &rhs)const
	{
		return (rhs - *this).LengthSq();
	}

	// Normalize
	AEVec2		AEVec2::Normalize() const
	{
		float l = Length();
		if (FLOAT_ZERO(l))
			return AEVec2();

		return (*this) / l;
	}
	const AEVec2 & AEVec2::NormalizeThis()
	{
		*this = this->Normalize();
		return *this;
	}

	// dot product
	float			AEVec2::Dot(const AEVec2& rhs) const
	{
		return x * rhs.x + y *rhs.y;
	}
	float			AEVec2::operator*		(const AEVec2& rhs) const
	{
		return Dot(rhs);
	}

	//Just multiply both
	AEVec2	AEVec2::multiply(const AEVec2& rhs) const
	{
		return AEVec2(x * rhs.x, y * rhs.y);
	}

	// projection
	AEVec2		AEVec2::Project(const AEVec2& rhs)const
	{

		AEVec2 n = rhs.Normalize();
		AEVec2 res = n * (this->Dot(n));
		return res;
	}
	AEVec2		AEVec2::ProjectPerp(const AEVec2& rhs)const
	{

		AEVec2 proj = Project(rhs);
		proj = *this - proj;
		return proj;
	}

	// cross product magnitude
	float			AEVec2::CrossMag(const AEVec2& rhs)const
	{
		return x * rhs.y - rhs.x * y;
	}

	// To and From Angle
	float			AEVec2::GetAngle()const
	{
		AEVec2 n = Normalize();
		return atan2f(n.y, n.x);
	}
	void		AEVec2::FromAngle(float angle)
	{
		x = cosf(angle);
		y = sinf(angle);
	}

	// ---------------------------------------------------------------------------
	AEVec2		AEVec2::Perp() const
	{
		return AEVec2(-y, x);
	}
	void	Perp(AEVec2* pResult, AEVec2* pVec0)
	{
		float temp = pVec0->x;
		pResult->x = -pVec0->y;
		pResult->y = temp;
	}

	AEVec2 AEVec2::Lerp(AEVec2 from, AEVec2 to, float tn)
	{
		return from + (to - from) * (1 - tn);
	}
	AEVec2 AEVec2::Random(AEVec2 min, AEVec2 max)
	{
		float rx = RandFloat(min.x, max.x);
		float ry = RandFloat(min.y, max.y);
		return AEVec2(rx, ry);
	}
}
// ---------------------------------------------------------------------------
