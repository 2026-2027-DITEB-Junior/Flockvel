#include "AEXMath.h"
#include <stdio.h>	// printf
#include <cstring>	// memeset

namespace aex
{
	// ---------------------------------------------------------------------------
	// constructors
	AEMtx44::AEMtx44()
	{
		memset(this->v, 0, sizeof(float) * 16);
	}

	// ---------------------------------------------------------------------------

	AEMtx44::AEMtx44(const float* mm)
	{
		RowCol(0, 0) = mm[0]; RowCol(0, 1) = mm[1]; RowCol(0, 2) = mm[2]; RowCol(0, 3) = mm[3];
		RowCol(1, 0) = mm[4]; RowCol(1, 1) = mm[5]; RowCol(1, 2) = mm[6]; RowCol(1, 3) = mm[7];
		RowCol(2, 0) = mm[8]; RowCol(2, 1) = mm[9]; RowCol(2, 2) = mm[10]; RowCol(2, 3) = mm[11];
		RowCol(3, 0) = mm[12]; RowCol(3, 1) = mm[13]; RowCol(3, 2) = mm[14]; RowCol(3, 3) = mm[15];
	}

	// ---------------------------------------------------------------------------

	AEMtx44::AEMtx44(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		RowCol(0, 0) = m00; RowCol(0, 1) = m01; RowCol(0, 2) = m02; RowCol(0, 3) = m03;
		RowCol(1, 0) = m10; RowCol(1, 1) = m11; RowCol(1, 2) = m12; RowCol(1, 3) = m13;
		RowCol(2, 0) = m20; RowCol(2, 1) = m21; RowCol(2, 2) = m22; RowCol(2, 3) = m23;
		RowCol(3, 0) = m30; RowCol(3, 1) = m31; RowCol(3, 2) = m32; RowCol(3, 3) = m33;
	}

	// ---------------------------------------------------------------------------

	AEMtx44::AEMtx44(const AEMtx44& rhs)
	{
		(*this) = rhs;
	}

	// ---------------------------------------------------------------------------

	AEMtx44::AEMtx44(const AEMtx33&rhs)
	{
		IdentityThis();
		this->m00 = rhs.m11;
		this->m01 = rhs.m12;
		this->m02 = rhs.m13;

		this->m10 = rhs.m21;
		this->m11 = rhs.m22;
		this->m12 = rhs.m23;

		this->m20 = rhs.m31;
		this->m21 = rhs.m32;
		this->m22 = rhs.m33;

	}

	// ---------------------------------------------------------------------------

	AEMtx44::operator AEMtx33() const
	{
		AEMtx33 mtx;
		for (unsigned int i = 0; i < 3; ++i)
		for (unsigned int j = 0; j < 3; ++j)
			mtx.m[i][j] = RowCol(i,j);
		return mtx;
	}


	// ---------------------------------------------------------------------------

	AEMtx44& AEMtx44::operator=(const AEMtx33&rhs)
	{
		IdentityThis();

		this->m00 = rhs.m11;
		this->m01 = rhs.m12;
		this->m03 = rhs.m13;

		this->m10 = rhs.m21;
		this->m11 = rhs.m22;
		this->m13 = rhs.m23;

		this->m20 = rhs.m31;
		this->m21 = rhs.m32;
		this->m23 = rhs.m33;

		return *this;
	}

	// ---------------------------------------------------------------------------
	// operators overload

	AEMtx44 AEMtx44::operator+(const AEMtx44& rhs) const
	{
		return AEMtx44(RowCol(0, 0) + rhs.RowCol(0, 0), RowCol(0, 1) + rhs.RowCol(0, 1), RowCol(0, 2) + rhs.RowCol(0, 2), RowCol(0, 3) + rhs.RowCol(0, 3),
			RowCol(1, 0) + rhs.RowCol(1, 0), RowCol(1, 1) + rhs.RowCol(1, 1), RowCol(1, 2) + rhs.RowCol(1, 2), RowCol(1, 3) + rhs.RowCol(1, 3),
			RowCol(2, 0) + rhs.RowCol(2, 0), RowCol(2, 1) + rhs.RowCol(2, 1), RowCol(2, 2) + rhs.RowCol(2, 2), RowCol(2, 3) + rhs.RowCol(2, 3),
			RowCol(3, 0) + rhs.RowCol(3, 0), RowCol(3, 1) + rhs.RowCol(3, 1), RowCol(3, 2) + rhs.RowCol(3, 2), RowCol(3, 3) + rhs.RowCol(3, 3));
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::operator-(const AEMtx44& rhs) const
	{
		return AEMtx44(RowCol(0, 0) - rhs.RowCol(0, 0), RowCol(0, 1) - rhs.RowCol(0, 1), RowCol(0, 2) - rhs.RowCol(0, 2), RowCol(0, 3) - rhs.RowCol(0, 3),
			RowCol(1, 0) - rhs.RowCol(1, 0), RowCol(1, 1) - rhs.RowCol(1, 1), RowCol(1, 2) - rhs.RowCol(1, 2), RowCol(1, 3) - rhs.RowCol(1, 3),
			RowCol(2, 0) - rhs.RowCol(2, 0), RowCol(2, 1) - rhs.RowCol(2, 1), RowCol(2, 2) - rhs.RowCol(2, 2), RowCol(2, 3) - rhs.RowCol(2, 3),
			RowCol(3, 0) - rhs.RowCol(3, 0), RowCol(3, 1) - rhs.RowCol(3, 1), RowCol(3, 2) - rhs.RowCol(3, 2), RowCol(3, 3) - rhs.RowCol(3, 3));
	}

	// ---------------------------------------------------------------------------

	AEVec3 AEMtx44::operator*(const AEVec3& v) const
	{
		return MultVec(v);
	}

	// ---------------------------------------------------------------------------

	AEVec3 AEMtx44::operator/(const AEVec3& v) const
	{
		return MultVecSR(v);
	}

	// ---------------------------------------------------------------------------

	AEVec4 AEMtx44::operator* (const AEVec4& v) const
	{
		return MultVec(v);
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::operator*(float s) const
	{
		return this->Mult(s);
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::operator*(const AEMtx44& rhs) const
	{
		return (*this).Mult(rhs);
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::operator/(float s) const
	{
		return (*this).Mult(1.0f / s);
	}

	// ---------------------------------------------------------------------------

	void AEMtx44::Print() const
	{
		for (unsigned int i = 0; i < 16; i++)
		{
			printf("%8f ", v[i]);

			if (((i + 1) & 0x03) == 0)
				printf("\n");
		}
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::Mult(float s) const
	{
		return AEMtx44(s * RowCol(0, 0), s * RowCol(0, 1), s * RowCol(0, 2), s * RowCol(0, 3),
			s * RowCol(1, 0), s * RowCol(1, 1), s * RowCol(1, 2), s * RowCol(1, 3),
			s * RowCol(2, 0), s * RowCol(2, 1), s * RowCol(2, 2), s * RowCol(2, 3),
			s * RowCol(3, 0), s * RowCol(3, 1), s * RowCol(3, 2), s * RowCol(3, 3));
	}

	// ---------------------------------------------------------------------------

	AEMtx44& AEMtx44::MultThis(float s)
	{
		RowCol(0, 0) *= s; RowCol(0, 1) *= s; RowCol(0, 2) *= s; RowCol(0, 3) *= s;
		RowCol(1, 0) *= s; RowCol(1, 1) *= s; RowCol(1, 2) *= s; RowCol(1, 3) *= s;
		RowCol(2, 0) *= s; RowCol(2, 1) *= s; RowCol(2, 2) *= s; RowCol(2, 3) *= s;
		RowCol(3, 0) *= s; RowCol(3, 1) *= s; RowCol(3, 2) *= s; RowCol(3, 3) *= s;

		return (*this);
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::Mult(const AEMtx44& rhs) const
	{
		return AEMtx44(
			RowCol(0, 0) * rhs.RowCol(0, 0) + RowCol(0, 1) * rhs.RowCol(1, 0) + RowCol(0, 2) * rhs.RowCol(2, 0) + RowCol(0, 3) * rhs.RowCol(3, 0),
			RowCol(0, 0) * rhs.RowCol(0, 1) + RowCol(0, 1) * rhs.RowCol(1, 1) + RowCol(0, 2) * rhs.RowCol(2, 1) + RowCol(0, 3) * rhs.RowCol(3, 1),
			RowCol(0, 0) * rhs.RowCol(0, 2) + RowCol(0, 1) * rhs.RowCol(1, 2) + RowCol(0, 2) * rhs.RowCol(2, 2) + RowCol(0, 3) * rhs.RowCol(3, 2),
			RowCol(0, 0) * rhs.RowCol(0, 3) + RowCol(0, 1) * rhs.RowCol(1, 3) + RowCol(0, 2) * rhs.RowCol(2, 3) + RowCol(0, 3) * rhs.RowCol(3, 3),

			RowCol(1, 0) * rhs.RowCol(0, 0) + RowCol(1, 1) * rhs.RowCol(1, 0) + RowCol(1, 2) * rhs.RowCol(2, 0) + RowCol(1, 3) * rhs.RowCol(3, 0),
			RowCol(1, 0) * rhs.RowCol(0, 1) + RowCol(1, 1) * rhs.RowCol(1, 1) + RowCol(1, 2) * rhs.RowCol(2, 1) + RowCol(1, 3) * rhs.RowCol(3, 1),
			RowCol(1, 0) * rhs.RowCol(0, 2) + RowCol(1, 1) * rhs.RowCol(1, 2) + RowCol(1, 2) * rhs.RowCol(2, 2) + RowCol(1, 3) * rhs.RowCol(3, 2),
			RowCol(1, 0) * rhs.RowCol(0, 3) + RowCol(1, 1) * rhs.RowCol(1, 3) + RowCol(1, 2) * rhs.RowCol(2, 3) + RowCol(1, 3) * rhs.RowCol(3, 3),

			RowCol(2, 0) * rhs.RowCol(0, 0) + RowCol(2, 1) * rhs.RowCol(1, 0) + RowCol(2, 2) * rhs.RowCol(2, 0) + RowCol(2, 3) * rhs.RowCol(3, 0),
			RowCol(2, 0) * rhs.RowCol(0, 1) + RowCol(2, 1) * rhs.RowCol(1, 1) + RowCol(2, 2) * rhs.RowCol(2, 1) + RowCol(2, 3) * rhs.RowCol(3, 1),
			RowCol(2, 0) * rhs.RowCol(0, 2) + RowCol(2, 1) * rhs.RowCol(1, 2) + RowCol(2, 2) * rhs.RowCol(2, 2) + RowCol(2, 3) * rhs.RowCol(3, 2),
			RowCol(2, 0) * rhs.RowCol(0, 3) + RowCol(2, 1) * rhs.RowCol(1, 3) + RowCol(2, 2) * rhs.RowCol(2, 3) + RowCol(2, 3) * rhs.RowCol(3, 3),

			RowCol(3, 0) * rhs.RowCol(0, 0) + RowCol(3, 1) * rhs.RowCol(1, 0) + RowCol(3, 2) * rhs.RowCol(2, 0) + RowCol(3, 3) * rhs.RowCol(3, 0),
			RowCol(3, 0) * rhs.RowCol(0, 1) + RowCol(3, 1) * rhs.RowCol(1, 1) + RowCol(3, 2) * rhs.RowCol(2, 1) + RowCol(3, 3) * rhs.RowCol(3, 1),
			RowCol(3, 0) * rhs.RowCol(0, 2) + RowCol(3, 1) * rhs.RowCol(1, 2) + RowCol(3, 2) * rhs.RowCol(2, 2) + RowCol(3, 3) * rhs.RowCol(3, 2),
			RowCol(3, 0) * rhs.RowCol(0, 3) + RowCol(3, 1) * rhs.RowCol(1, 3) + RowCol(3, 2) * rhs.RowCol(2, 3) + RowCol(3, 3) * rhs.RowCol(3, 3));
	}

	// ---------------------------------------------------------------------------

	AEMtx44& AEMtx44::MultThis(const AEMtx44& rhs)
	{
		return (*this) = this->Mult(rhs);
	}

	// ---------------------------------------------------------------------------

	AEVec3 AEMtx44::MultVec(const AEVec3& v) const
	{
		return AEVec3(
			RowCol(0, 0) * v.x + RowCol(0, 1) * v.y + RowCol(0, 2) * v.z + RowCol(0, 3),
			RowCol(1, 0) * v.x + RowCol(1, 1) * v.y + RowCol(1, 2) * v.z + RowCol(1, 3),
			RowCol(2, 0) * v.x + RowCol(2, 1) * v.y + RowCol(2, 2) * v.z + RowCol(2, 3));
	}

	// ---------------------------------------------------------------------------

	AEVec3 AEMtx44::MultVecSR(const AEVec3& v) const
	{
		return AEVec3(
			RowCol(0, 0) * v.x + RowCol(0, 1) * v.y + RowCol(0, 2) * v.z,
			RowCol(1, 0) * v.x + RowCol(1, 1) * v.y + RowCol(1, 2) * v.z,
			RowCol(2, 0) * v.x + RowCol(2, 1) * v.y + RowCol(2, 2) * v.z);
	}

	// ---------------------------------------------------------------------------

	AEVec4 AEMtx44::MultVec(const AEVec4& v) const
	{
		return AEVec4(
			RowCol(0, 0) * v.x + RowCol(0, 1) * v.y + RowCol(0, 2) * v.z + RowCol(0, 3) * v.w,
			RowCol(1, 0) * v.x + RowCol(1, 1) * v.y + RowCol(1, 2) * v.z + RowCol(1, 3) * v.w,
			RowCol(2, 0) * v.x + RowCol(2, 1) * v.y + RowCol(2, 2) * v.z + RowCol(2, 3) * v.w,
			v.w);
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::Transpose() const
	{
		AEMtx44 res;

		res.RowCol(0, 0) = RowCol(0, 0); res.RowCol(0, 1) = RowCol(1, 0); res.RowCol(0, 2) = RowCol(2, 0); res.RowCol(0, 3) = RowCol(3, 0);
		res.RowCol(1, 0) = RowCol(0, 1); res.RowCol(1, 1) = RowCol(1, 1); res.RowCol(1, 2) = RowCol(2, 1); res.RowCol(1, 3) = RowCol(3, 1);
		res.RowCol(2, 0) = RowCol(0, 2); res.RowCol(2, 1) = RowCol(1, 2); res.RowCol(2, 2) = RowCol(2, 2); res.RowCol(2, 3) = RowCol(3, 2);
		res.RowCol(3, 0) = RowCol(0, 3); res.RowCol(3, 1) = RowCol(1, 3); res.RowCol(3, 2) = RowCol(2, 3); res.RowCol(3, 3) = RowCol(3, 3);

		return res;
	}

	// ---------------------------------------------------------------------------

	AEMtx44& AEMtx44::TransposeThis()
	{
		float f;

		// swap the 1st row
		f = RowCol(0, 1); RowCol(0, 1) = RowCol(1, 0); RowCol(1, 0) = f;
		f = RowCol(0, 2); RowCol(0, 2) = RowCol(2, 0); RowCol(2, 0) = f;
		f = RowCol(0, 3); RowCol(0, 3) = RowCol(3, 0); RowCol(3, 0) = f;

		// swap the 2nd row
		f = RowCol(1, 2); RowCol(1, 2) = RowCol(2, 1); RowCol(2, 1) = f;
		f = RowCol(1, 3); RowCol(1, 3) = RowCol(3, 1); RowCol(3, 1) = f;

		// swap the 3rd row
		f = RowCol(2, 3); RowCol(2, 3) = RowCol(3, 2); RowCol(3, 2) = f;

		return (*this);
	}

	// ---------------------------------------------------------------------------

	// Extract the angles from the matrix
	void AEMtx44::GetEulerAnglesXYZ(float & x, float & y, float & z) const
	{
		float cX, cY, cZ, sX, sY, sZ;

		sY = -this->v[8];
		cY = sqrtf(1 - sY*sY);

		// check if cY is zero - if so assume rotation around X is 0.0
		if (FLOAT_ZERO(cY))
		{
			sX = 0.0f;
			cX = 1.0f;
			sZ = this->v[1];
			cZ = this->v[5];
		}
		else // use default algorithm
		{
			sX = this->v[9] / cY;
			cX = this->v[10] / cY;
			sZ = this->v[4] / cY;
			cZ = this->v[0] / cY;
		}

		// call arctan2
		x = atan2f(sX, cX);
		y = atan2f(sY, cY);
		z = atan2f(sZ, cZ);
	}
	void AEMtx44::GetEulerAnglesZYX(float & x, float & y, float & z) const
	{
		float cX, cY, cZ, sX, sY, sZ;

		sY = this->v[2];
		cY = sqrtf(1 - sY*sY);

		// check if cY is zero - if so assume rotation around Z is 0.0
		if (FLOAT_ZERO(cY))
		{
			sZ = 0.0f;
			cZ = 1.0f;
			sX = this->v[9];
			cX = this->v[5];
		}
		else // use default algorithm
		{
			sX = -this->v[6] / cY;
			cX = this->v[10] / cY;
			sZ = -this->v[1] / cY;
			cZ = this->v[0] / cY;
		}

		// call arctan2
		x = atan2f(sX, cX);
		y = atan2f(sY, cY);
		z = atan2f(sZ, cZ);
	}

	// ---------------------------------------------------------------------------

	// the following functions will be applied to 'this' matrix
	AEMtx44& AEMtx44::ZeroThis()
	{
		RowCol(0, 0) = RowCol(0, 1) = RowCol(0, 2) = RowCol(0, 3) =
			RowCol(1, 0) = RowCol(1, 1) = RowCol(1, 2) = RowCol(1, 3) =
			RowCol(2, 0) = RowCol(2, 1) = RowCol(2, 2) = RowCol(2, 3) =
			RowCol(3, 0) = RowCol(3, 1) = RowCol(3, 2) = RowCol(3, 3) = 0.0f;

		return (*this);
	}

	// ---------------------------------------------------------------------------

	AEMtx44& AEMtx44::IdentityThis()
	{
		RowCol(0, 1) = RowCol(0, 2) = RowCol(0, 3) =
			RowCol(1, 0) = RowCol(1, 2) = RowCol(1, 3) =
			RowCol(2, 0) = RowCol(2, 1) = RowCol(2, 3) =
			RowCol(3, 0) = RowCol(3, 1) = RowCol(3, 2) = 0.0f;

		RowCol(0, 0) = RowCol(1, 1) = RowCol(2, 2) = RowCol(3, 3) = 1.0f;

		return (*this);
	}

	// ---------------------------------------------------------------------------

	AEMtx44& AEMtx44::ScaleThis(float x, float y, float z)
	{
		RowCol(0, 0) *= x; RowCol(0, 1) *= x; RowCol(0, 2) *= x; RowCol(0, 3) *= x;
		RowCol(1, 0) *= y; RowCol(1, 1) *= y; RowCol(1, 2) *= y; RowCol(1, 3) *= y;
		RowCol(2, 0) *= z; RowCol(2, 1) *= z; RowCol(2, 2) *= z; RowCol(2, 3) *= z;

		return (*this);
	}

	// ---------------------------------------------------------------------------

	AEMtx44&	AEMtx44::RotateThis(float x, float y, float z)
	{
		// create the rotation matrix
		AEMtx44 other = AEMtx44::Rotate(x, y, z);

		// applty rotation
		*this = other * (*this);

		return *this;
	}

	// ---------------------------------------------------------------------------

	AEMtx44& AEMtx44::RotateThis(float x, float y, float z, float angle)
	{
		return (*this) = AEMtx44::Rotate(x, y, z, angle) * (*this);
	}

	// ---------------------------------------------------------------------------

	AEMtx44& AEMtx44::TranslateThis(float x, float y, float z)
	{
		return (*this) = AEMtx44::Translate(x, y, z) * (*this);
	}

	// ---------------------------------------------------------------------------
	// the following functions construct a matrix

	AEMtx44 AEMtx44::Zero()
	{
		static AEMtx44 res(
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f);

		return res;
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::Identity()
	{
		static AEMtx44 res(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		return res;
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::Scale(float x, float y, float z)
	{
		return AEMtx44(
			x, 0.0f, 0.0f, 0.0f,
			0.0f, y, 0.0f, 0.0f,
			0.0f, 0.0f, z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::Rotate(float x, float y, float z)
	{
		return RotateZYX(x, y, z);
	}

	AEMtx44	AEMtx44::RotateZYX(float x, float y, float z) // Z-Y-X Euler
	{
		// compute cosines and sines
		float cX = cosf(x); float sX = sinf(x);
		float cY = cosf(y); float sY = sinf(y);
		float cZ = cosf(z); float sZ = sinf(z);

		// Create the rotation matrix 
		AEMtx44 mtx;

		mtx.v[0] = cY * cZ;
		mtx.v[1] = -cY * sZ;
		mtx.v[2] = sY;
		mtx.v[3] = 0.0f;

		mtx.v[4] = sX*sY*cZ + cX*sZ;
		mtx.v[5] = -sX*sY*sZ + cX*cZ;
		mtx.v[6] = -sX*cY;
		mtx.v[7] = 0.0f;

		mtx.v[8] = -cX*sY*cZ + sX*sZ;
		mtx.v[9] = cX*sY*sZ + sX*cZ;
		mtx.v[10] = cX*cY;
		mtx.v[11] = 0.0f;

		mtx.v[12] = 0.0f;
		mtx.v[13] = 0.0f;
		mtx.v[14] = 0.0f;
		mtx.v[15] = 1.0f;

		return mtx;
	}
	AEMtx44	AEMtx44::RotateXYZ(float x, float y, float z) // X-Y-Z Euler
	{
		// compute cosines and sines
		float cX = cosf(x); float sX = sinf(x);
		float cY = cosf(y); float sY = sinf(y);
		float cZ = cosf(z); float sZ = sinf(z);

		// Create the rotation matrix 
		AEMtx44 mtx;

		// row 1
		mtx.v[0] = cY * cZ;
		mtx.v[1] = cZ*sY*sX - sZ*cX;
		mtx.v[2] = cZ*sY*cX + sZ*sX;
		mtx.v[3] = 0.0f;

		// row 2
		mtx.v[4] = sZ*cY;
		mtx.v[5] = sZ*sY*sX + cZ*cX;
		mtx.v[6] = sZ*sY*cX - cZ*sX;
		mtx.v[7] = 0.0f;

		// row 3
		mtx.v[8] = -sY;
		mtx.v[9] = cY*sX;
		mtx.v[10] = cY*cX;
		mtx.v[11] = 0.0f;

		// row 4
		mtx.v[12] = 0.0f;
		mtx.v[13] = 0.0f;
		mtx.v[14] = 0.0f;
		mtx.v[15] = 1.0f;

		return mtx;
	}
	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::Rotate(float x, float y, float z, float angle)
	{
		float c = cos(angle);
		float t = 1.0f - c;
		float s = sin(angle);

		float xx = x * x;
		float xy = x * y;
		float xz = x * z;
		float yy = y * y;
		float yz = y * z;
		float zz = z * z;

		AEMtx44 m;
		m.m[0][0] = t * xx + c;
		m.m[0][1] = t * xy - s * z;
		m.m[0][2] = t * xz + s * y;
		m.m[0][3] = 0.0f;

		m.m[1][0] = t * xy + s * z;
		m.m[1][1] = t * yy + c;
		m.m[1][2] = t * yz - s * x;
		m.m[1][3] = 0.0f;

		m.m[2][0] = t * xz - s * y;
		m.m[2][1] = t * yz + s * x;
		m.m[2][2] = t*zz + c;
		m.m[2][3] = 0.0f;

		m.m[3][0] = 0.0f;
		m.m[3][1] = 0.0f;
		m.m[3][2] = 0.0f;
		m.m[3][3] = 1.0f;

		return m;

/*		AEMtx44 A(
			c, 0.0f, 0.0f, 0.0f,
			0.0f, c, 0.0f, 0.0f,
			0.0f, 0.0f, c, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		AEMtx44 B(
			xx, xy, xz, 0.0f,
			xy, yy, yz, 0.0f,
			xz, yz, zz, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f);
		AEMtx44 C(
			0.0f, -z, y, 0.0f,
			z, 0.0f, -x, 0.0f,
			-y, x, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f);

		return A + t * B + s * C;*/
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::Translate(float x, float y, float z)
	{
		return AEMtx44(
			1.0f, 0.0f, 0.0f, x,
			0.0f, 1.0, 0.0f, y,
			0.0f, 0.0f, 1.0, z,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::ScaleInverse(float x, float y, float z)
	{
		return AEMtx44(
			1.0f / x, 0.0f,		0.0f,	  0.0f,
			0.0f,	  1.0f / y, 0.0f,	  0.0f,
			0.0f,	  0.0f,		1.0f / z, 0.0f,
			0.0f,	  0.0f,		0.0f,	  1.0f);
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::RotateInverse(float x, float y, float z)
	{
		return RotateZYXInverse(x, y, z);
	}

	AEMtx44	AEMtx44::RotateZYXInverse(float x, float y, float z) // Z-Y-X Euler
	{
		// compute cosines and sines
		float cX = cosf(x); float sX = -sinf(x);
		float cY = cosf(y); float sY = -sinf(y);
		float cZ = cosf(z); float sZ = -sinf(z);

		// Create the rotation matrix 
		AEMtx44 mtx;

		mtx.v[0] = cY * cZ;
		mtx.v[1] = -cY * sZ;
		mtx.v[2] = sY;
		mtx.v[3] = 0.0f;

		mtx.v[4] = sX * sY * cZ + cX * sZ;
		mtx.v[5] = -sX * sY * sZ + cX * cZ;
		mtx.v[6] = -sX * cY;
		mtx.v[7] = 0.0f;

		mtx.v[8] = -cX * sY * cZ + sX * sZ;
		mtx.v[9] = cX * sY * sZ + sX * cZ;
		mtx.v[10] = cX * cY;
		mtx.v[11] = 0.0f;

		mtx.v[12] = 0.0f;
		mtx.v[13] = 0.0f;
		mtx.v[14] = 0.0f;
		mtx.v[15] = 1.0f;

		return mtx;
	}
	AEMtx44	AEMtx44::RotateXYZInverse(float x, float y, float z) // X-Y-Z Euler
	{
		// compute cosines and sines
		float cX = cosf(x); float sX = -sinf(x);
		float cY = cosf(y); float sY = -sinf(y);
		float cZ = cosf(z); float sZ = -sinf(z);

		// Create the rotation matrix 
		AEMtx44 mtx;

		// row 1
		mtx.v[0] = cY * cZ;
		mtx.v[1] = cZ * sY * sX - sZ * cX;
		mtx.v[2] = cZ * sY * cX + sZ * sX;
		mtx.v[3] = 0.0f;

		// row 2
		mtx.v[4] = sZ * cY;
		mtx.v[5] = sZ * sY * sX + cZ * cX;
		mtx.v[6] = sZ * sY * cX - cZ * sX;
		mtx.v[7] = 0.0f;

		// row 3
		mtx.v[8] = -sY;
		mtx.v[9] = cY * sX;
		mtx.v[10] = cY * cX;
		mtx.v[11] = 0.0f;

		// row 4
		mtx.v[12] = 0.0f;
		mtx.v[13] = 0.0f;
		mtx.v[14] = 0.0f;
		mtx.v[15] = 1.0f;

		return mtx;
	}
	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::RotateInverse(float x, float y, float z, float angle)
	{
		angle = -angle;

		float c = cos(angle);
		float t = 1.0f - c;
		float s = sin(angle);

		float xx = x * x;
		float xy = x * y;
		float xz = x * z;
		float yy = y * y;
		float yz = y * z;
		float zz = z * z;

		AEMtx44 m;
		m.m[0][0] = t * xx + c;
		m.m[0][1] = t * xy - s * z;
		m.m[0][2] = t * xz + s * y;
		m.m[0][3] = 0.0f;

		m.m[1][0] = t * xy + s * z;
		m.m[1][1] = t * yy + c;
		m.m[1][2] = t * yz - s * x;
		m.m[1][3] = 0.0f;

		m.m[2][0] = t * xz - s * y;
		m.m[2][1] = t * yz + s * x;
		m.m[2][2] = t * zz + c;
		m.m[2][3] = 0.0f;

		m.m[3][0] = 0.0f;
		m.m[3][1] = 0.0f;
		m.m[3][2] = 0.0f;
		m.m[3][3] = 1.0f;

		return m;

		/*		AEMtx44 A(
					c, 0.0f, 0.0f, 0.0f,
					0.0f, c, 0.0f, 0.0f,
					0.0f, 0.0f, c, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
				AEMtx44 B(
					xx, xy, xz, 0.0f,
					xy, yy, yz, 0.0f,
					xz, yz, zz, 0.0f,
					0.0f, 0.0f, 0.0f, 0.0f);
				AEMtx44 C(
					0.0f, -z, y, 0.0f,
					z, 0.0f, -x, 0.0f,
					-y, x, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 0.0f);

				return A + t * B + s * C;*/
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::TranslateInverse(float x, float y, float z)
	{
		return AEMtx44(
			1.0f, 0.0f, 0.0f, -x,
			0.0f, 1.0, 0.0f,  -y,
			0.0f, 0.0f, 1.0,  -z,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	// ---------------------------------------------------------------------------
	// calculate a look at matrix
	// * camera is looking along -z axis and y is up
	AEMtx44 AEMtx44::LookAt(float posX, float posY, float posZ, float targetX, float targetY, float targetZ, float upX, float upY, float upZ)
	{
		AEVec3 pos = AEVec3(posX, posY, posZ);
		AEVec3 target = AEVec3(targetX, targetY, targetZ);
		AEVec3 up = AEVec3(upX, upY, upZ);

		// compute view axis
		AEVec3 dir = (pos - target).Normalize();

		// compute side axis = dir X up
		AEVec3 side = up.Cross(dir).Normalize();

		// compute up axis = dir X side
		AEVec3 up_loc = dir.Cross(side).Normalize();

		// matrix
		AEMtx44 m;
		m.RowCol(0, 0) = side.x;		m.RowCol(0, 1) = side.y;		m.RowCol(0, 2) = side.z;		m.RowCol(0, 3) = -(side * pos);
		m.RowCol(1, 0) = up_loc.x;	m.RowCol(1, 1) = up_loc.y;	m.RowCol(1, 2) = up_loc.z;	m.RowCol(1, 3) = -(up_loc * pos);
		m.RowCol(2, 0) = dir.x;		m.RowCol(2, 1) = dir.y;		m.RowCol(2, 2) = dir.z;		m.RowCol(2, 3) = -(dir * pos);
		m.RowCol(3, 0) = 0.0f;		m.RowCol(3, 1) = 0.0f;		m.RowCol(3, 2) = 0.0f;		m.RowCol(3, 3) = 1.0f;
		return m;
	}

	// ---------------------------------------------------------------------------

	AEMtx44 AEMtx44::LookAt(const AEVec3& pos, const AEVec3& target, const AEVec3& up)
	{
		return LookAt(
			pos.x, pos.y, pos.z,
			target.x, target.y, target.z,
			up.x, up.y, up.z);
	}

	// ---------------------------------------------------------------------------

	AEMtx44	AEMtx44::InvLookAt(const AEVec3& pos, const AEVec3& target, const AEVec3& up)
	{

		// compute view axis
		AEVec3 dir = (pos - target).Normalize();

		// compute side axis = dir X up
		AEVec3 side = up.Cross(dir).Normalize();

		// compute up axis = dir X side
		AEVec3 up_loc = dir.Cross(side).Normalize();

		// matrix
		AEMtx44 m;
		m.RowCol(0, 0) = side.x;		m.RowCol(0, 1) = up_loc.x;	m.RowCol(0, 2) = dir.x;		m.RowCol(0, 3) = pos.x;
		m.RowCol(1, 0) = side.y;		m.RowCol(1, 1) = up_loc.y;	m.RowCol(1, 2) = dir.y;		m.RowCol(1, 3) = pos.y;
		m.RowCol(2, 0) = side.z;		m.RowCol(2, 1) = up_loc.z;	m.RowCol(2, 2) = dir.z;		m.RowCol(2, 3) = pos.z;
		m.RowCol(3, 0) = 0.0f;		m.RowCol(3, 1) = 0.0f;		m.RowCol(3, 2) = 0.0f;		m.RowCol(3, 3) = 1.0f;

		return m;
	}

	// ---------------------------------------------------------------------------

	AEMtx44	AEMtx44::OrthoProj(float width, float height, float near_val, float far_val)
	{

		AEMtx44 proj = AEMtx44();

		float view_depth = near_val - far_val;

		proj.RowCol(0, 0) = 2.0f / width;
		proj.RowCol(1, 1) = 2.0f / height;
		proj.RowCol(2, 2) = 1.0f / view_depth;
		proj.RowCol(2, 3) = near_val / view_depth;
		proj.RowCol(3, 3) = 1.0f;

		return proj;
	}
	AEMtx44	AEMtx44::OrthoProjGL(float width, float height, float near_val, float far_val)
	{
		AEMtx44 proj = AEMtx44();

		float view_depth = far_val - near_val;

		proj.RowCol(0, 0) = 2.0f / width;
		proj.RowCol(1, 1) = 2.0f / height;
		proj.RowCol(2, 2) = -2.0f / view_depth;
		proj.RowCol(2, 3) = -(far_val + near_val) / view_depth;
		proj.RowCol(3, 3) = 1.0f;

		return proj;
	}

	// ---------------------------------------------------------------------------

	AEMtx44	AEMtx44::PerspProj(float fovY, float ratio, float near_val, float far_val)
	{
		// cot(x) = tan( PI / 2 - x)
		float d = tanf(HALF_PI - (fovY / 2.0f));
		float a = ratio;

		AEMtx44 m;

		// make projection
		m.RowCol(0, 0) = d / a;
		m.RowCol(1, 1) = d;
		m.RowCol(2, 2) = (near_val + far_val) / (near_val - far_val);
		m.RowCol(2, 3) = (2 * near_val*far_val) / (near_val - far_val);
		m.RowCol(3, 2) = -1.0f;
		return m;
	}

	// ---------------------------------------------------------------------------

	AEMtx44	AEMtx44::PerspProj(float fovY, float width, float height, float near_val, float far_val)
	{
		return PerspProj(fovY, width / height, near_val, far_val);
	}
}