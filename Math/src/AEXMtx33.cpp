// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEMtx33.c
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	implementation of the 3x3 matrix library
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#include "AEXMath.h"

namespace aex
{
	// ---------------------------------------------------------------------------
	AEMtx33::AEMtx33()
	{
		SetIdentity();
	}
	AEMtx33::AEMtx33(float a11, float a12, float a13,
		float a21, float a22, float a23,
		float a31, float a32, float a33)
	{
		m11 = a11;	m21 = a21;	m31 = a31;
		m12 = a12;	m22 = a22;	m32 = a32;
		m13 = a13;	m23 = a23;	m33 = a33;
	}

	// Set this matrix to identity
	void AEMtx33::SetIdentity()
	{
		for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			v[i * 3 + j] = (i == j) ? 1.0f : 0;
	}

	// return the transpose version of the matrix
	AEMtx33 AEMtx33::Transpose()const
	{
		AEMtx33 res;

		res.v[0] = v[0];
		res.v[1] = v[3];
		res.v[2] = v[6];

		res.v[3] = v[1];
		res.v[4] = v[4];
		res.v[5] = v[7];

		res.v[6] = v[2];
		res.v[7] = v[5];
		res.v[8] = v[8];

		return res;
	}
	AEMtx33& AEMtx33::TransposeThis()
	{
		return *this = Transpose();
	}

	// Matrix Concatenation
	AEMtx33 AEMtx33::Concat(const AEMtx33 & rhs)const
	{
		AEMtx33 res;

		res.v[0] = v[0] * rhs.v[0] + v[1] * rhs.v[3] + v[2] * rhs.v[6];
		res.v[1] = v[0] * rhs.v[1] + v[1] * rhs.v[4] + v[2] * rhs.v[7];
		res.v[2] = v[0] * rhs.v[2] + v[1] * rhs.v[5] + v[2] * rhs.v[8];

		res.v[3] = v[3] * rhs.v[0] + v[4] * rhs.v[3] + v[5] * rhs.v[6];
		res.v[4] = v[3] * rhs.v[1] + v[4] * rhs.v[4] + v[5] * rhs.v[7];
		res.v[5] = v[3] * rhs.v[2] + v[4] * rhs.v[5] + v[5] * rhs.v[8];

		res.v[6] = v[6] * rhs.v[0] + v[7] * rhs.v[3] + v[8] * rhs.v[6];
		res.v[7] = v[6] * rhs.v[1] + v[7] * rhs.v[4] + v[8] * rhs.v[7];
		res.v[8] = v[6] * rhs.v[2] + v[7] * rhs.v[5] + v[8] * rhs.v[8];

		return res;
	}
	AEMtx33 AEMtx33::operator *(const AEMtx33 & rhs)const
	{
		return Concat(rhs);
	}
	AEMtx33& AEMtx33::operator *=(const AEMtx33 &rhs)
	{
		*this = Concat(rhs);
		return *this;
	}

	// Vector multiplication
	AEVec2 AEMtx33::MultVec(const AEVec2 & vec)const
	{
		float x = v[0] * vec.x + v[1] * vec.y + v[2];
		float y = v[3] * vec.x + v[4] * vec.y + v[5];
		return AEVec2(x, y);
	}
	AEVec2 AEMtx33::MultVecDir(const AEVec2 &vec)const
	{
		float x = v[0] * vec.x + v[1] * vec.y;
		float y = v[3] * vec.x + v[4] * vec.y;
		return AEVec2(x, y);
	}
	AEVec2 AEMtx33::operator*(const AEVec2 & vec)const
	{
		return MultVec(vec);
	}

	// Static Interface
	AEMtx33 AEMtx33::Identity()
	{
		// return
		return AEMtx33();
	}
	AEMtx33 AEMtx33::Translate(float x, float y)
	{
		AEMtx33 tr;
		tr.v[2] = x;
		tr.v[5] = y;
		return tr;
	}
	AEMtx33 AEMtx33::Scale(float sx, float sy)
	{
		AEMtx33 sc;
		sc.v[0] = sx;
		sc.v[4] = sy;
		return sc;
	}
	AEMtx33 AEMtx33::RotDeg(float angle_deg)
	{
		return RotRad((angle_deg / 180.0f)*PI);
	}
	AEMtx33 AEMtx33::RotRad(float angle_rad)
	{
		AEMtx33 res;
		float cA = cosf(angle_rad);
		float sA = sinf(angle_rad);
		res.v[0] = cA;
		res.v[1] = -sA;
		res.v[3] = sA;
		res.v[4] = cA;
		return res;
	}
}