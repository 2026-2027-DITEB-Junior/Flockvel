#include "AEXMath.h"

namespace aex
{
	AEVec3::AEVec3()
	{
		x = y = z = 0.0f;
	}
	AEVec3::AEVec3(const AEVec3& rhs)
	{
		(*this) = rhs;
	}
	AEVec3::AEVec3(float xx, float yy, float zz)
	{
		x = xx, y = yy, z = zz;
	}
	AEVec3::AEVec3(float xx)
	{
		x = y = z = xx;
	}

	AEVec3::AEVec3(const AEVec2 &rhs2D, float zz)
	{
		x = rhs2D.x;
		y = rhs2D.y;
		z = zz;
	}

	AEVec3::operator AEVec2() const{
		return AEVec2(x, y);
	}

	// add or substract
	AEVec3		AEVec3::operator+		(const AEVec3& rhs) const
	{
		return AEVec3(x + rhs.x, y + rhs.y, z + rhs.z);
	}
	AEVec3&		AEVec3::operator+=		(const AEVec3& rhs)
	{
		*this = *this + rhs;
		return *this;
	}
	AEVec3		AEVec3::operator-		(const AEVec3& rhs) const
	{
		return AEVec3(x - rhs.x, y - rhs.y, z - rhs.z);
	}
	AEVec3&		AEVec3::operator-=		(const AEVec3& rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	// multiply or divide by a scalar
	AEVec3		AEVec3::operator*		(float s) const
	{
		return AEVec3(x * s, y * s, z * s);
	}
	AEVec3		AEVec3::operator/		(float s) const
	{
		return AEVec3(x / s, y / s, z / s);
	}
	AEVec3&		AEVec3::operator*=		(float s) 
	{
		*this = *this * s;
		return *this; 
	}
	AEVec3&		AEVec3::operator/=		(float s)
	{
		*this = *this / s;
		return *this;
	}

	// get vector length
	float			AEVec3::Length() const
	{
		return sqrtf((*this) * (*this));
	}

	float AEVec3::LengthSq() const
	{
		return *this * *this;
	}

	float AEVec3::Distance(const AEVec3& rhs) const
	{
		return (*this - rhs).Length();
	}

	float AEVec3::DistanceSq(const AEVec3& rhs) const
	{
		return (*this - rhs).LengthSq();
	}
	AEVec3		AEVec3::Project(const AEVec3& onto) const
	{
		AEVec3 n = onto.Normalize();
		AEVec3 res = n * (*this * n);
		return res;
	}

	// calculate normalized vector
	AEVec3		AEVec3::Normalize() const
	{
		return (*this) / Length();
	}
	const AEVec3&	AEVec3::NormalizeThis()
	{
		return (*this) = (*this) / Length();
	}

	// dot product
	float			AEVec3::operator*		(const AEVec3& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	// cross product
	AEVec3		AEVec3::Cross(const AEVec3& rhs) const
	{
		return AEVec3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
	}
}