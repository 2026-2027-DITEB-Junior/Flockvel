#include "AEXMath.h"
namespace aex
{
	AEVec4::AEVec4(const AEVec4& rhs)
	{
		(*this) = rhs;
	}

	AEVec4::AEVec4(const AEVec3& rhs3D, float ww) :
		x(rhs3D.x), y(rhs3D.y), z(rhs3D.z), w(ww) { }

	// ------------------------------------------------------------------------
	/// \fn		Default & custom
	/// \brief	
	AEVec4::AEVec4(float xx, float yy, float zz, float ww)
		: x(xx)
		, y(yy)
		, z(zz)
		, w(ww)
	{}

	AEVec3 AEVec4::vec3() const
	{
		return {x, y, z};
	}

	AEVec4 AEVec4::operator *(const AEVec4 & rhs)const
	{
		return AEVec4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
	}
	AEVec4& AEVec4::operator *=(const AEVec4 & rhs)
	{
		return *this = AEVec4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
	}
	AEVec4 AEVec4::operator *(const float & sc)const
	{
		return AEVec4(x * sc, y * sc, z * sc, w * sc);
	}
	AEVec4& AEVec4::operator *=(const float & sc)
	{
		return *this = AEVec4(x * sc, y * sc, z * sc, w * sc);
	}
	AEVec4 AEVec4::operator +(const AEVec4 & rhs)const
	{
		return AEVec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}
	AEVec4& AEVec4::operator += (const AEVec4 & rhs)
	{
		return *this = AEVec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}
	AEVec4 AEVec4::operator - (const AEVec4 & rhs)const
	{
		return AEVec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}
	AEVec4& AEVec4::operator -= (const AEVec4 & rhs)
	{
		return *this = AEVec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}

}// namespace aex