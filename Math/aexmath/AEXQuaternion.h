#ifndef AEX_QUATERNION_H_
#define AEX_QUATERNION_H_
// ----------------------------------------------------------------------------
#include "AEXMathDLL.h"
#include "AEXVec3.h"
#include "AEXMtx33.h"
#include "AEXMtx44.h"
#pragma warning (disable:4201) // nameless struct warning
namespace aex
{
	struct AEX_MATH_API Quaternion
	{
		union
		{
			struct
			{

				float x, y, z, w;
			};
			float v[4];
		};

		Quaternion();
		Quaternion(float xx, float yy, float zz, float ww);
		Quaternion(AEVec3 vec, float ww);
		Quaternion(AEVec3);


		// operators
		Quaternion operator+ (const Quaternion & rhs) const;
		Quaternion operator- (const Quaternion & rhs) const;
		Quaternion operator*(float scalar)const;
		Quaternion operator/(float scalar) const;
		Quaternion& operator+= (const Quaternion & rhs);
		Quaternion& operator-=(const Quaternion &rhs);
		Quaternion& operator*=(float scalar);
		Quaternion& operator/=(float scalar);

		// Left to right concatenation (q = this * rhs).
		Quaternion operator *(const Quaternion&rhs) const;
		Quaternion& operator*=(const Quaternion&rhs);

		// vector rotation
		AEVec3 operator*(const AEVec3 & rhs) const;

		// common operations
		void SetIdentity();
		void NormalizeThis();
		float Length()const ;
		Quaternion Normalize() const;
		float Dot(const Quaternion &rhs) const;
		Quaternion Inverse() const;

		// Format Conversion
		AEMtx33 ToMtx33() const;
		AEMtx44 ToMtx44() const;
		AEVec3	ToEulerXYZ() const;
		Quaternion& FromMtx33(const AEMtx33& mtx33);
		Quaternion& FromMtx44(const AEMtx44& mtx44);
		Quaternion& FromAxisAngle(const AEVec3 & axis, float angle_rad);
		Quaternion& FromEulerXYZ(float x_rad, float y_rad, float z_rad);

		// Static Quaternion Creation
		static Quaternion MakeAxisAngle(const AEVec3 &axis, float angle_rad);
		static Quaternion MakeEulerXYZ(float x_rad, float y_rad, float z_rad);
		static Quaternion MakeMatrix33(const AEMtx33 & rot33);
		static Quaternion MakeLookAt(const AEVec3& forward, const AEVec3& up);
	};
}
#pragma warning (default:4201) // nameless struct warning

// ----------------------------------------------------------------------------
#endif