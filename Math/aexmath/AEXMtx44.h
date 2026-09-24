#ifndef AEX_MTX44_H
#define AEX_MTX44_H

#include "AEXMathDLL.h"
#include "AEXMtx33.h"
#include "AEXVec3.h"

#pragma warning (disable:4201) // nameless struct warning

namespace aex
{
	// ---------------------------------------------------------------------------
	// a simple 4x4 matrix class

	struct AEX_MATH_API AEMtx44
	{
		union
		{
			// matrix data is stored row major
			float	v[16];
			float	m[4][4];
			struct
			{
				float		m00, m01, m02, m03,
					m10, m11, m12, m13,
					m20, m21, m22, m23,
					m30, m31, m32, m33;
			};
		};

		// constructor
		AEMtx44();
		AEMtx44(const float* mm);
		AEMtx44(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);

		AEMtx44(const AEMtx44& rhs);
		AEMtx44(const AEMtx33&rhs);
		AEMtx44 & operator=(const AEMtx33&rhs);

		// ---------------------------------------------------------------------------
		// accessors

		float&			RowCol(unsigned int row, unsigned int col)					{ return v[row * 4 + col]; }
		float				RowCol(unsigned int row, unsigned int col) const				{ return v[row * 4 + col]; }
		float&			operator()		(unsigned int row, unsigned int col)			{ return RowCol(row, col); }
		float				operator()		(unsigned int row, unsigned int col) const	{ return RowCol(row, col); }


		// ---------------------------------------------------------------------------
		// conversion to mtx33
		operator AEMtx33() const;


		// ---------------------------------------------------------------------------
		// operator overload

		// accessors
		AEMtx44			operator+		(const AEMtx44& rhs) const;

		AEMtx44			operator-		(const AEMtx44& rhs) const;

		AEMtx44			operator*		(float s) const;
		AEMtx44			operator*		(const AEMtx44& rhs) const;

		AEMtx44			operator/		(float s) const;

		// multiply with a 3D vector
		// * '*' assume w = 1.0
		// * '/' assume w = 0.0
		AEVec3			operator*		(const AEVec3& v) const;
		AEVec3			operator/		(const AEVec3& v) const;
		AEVec4			operator*		(const AEVec4& v) const;

		// ---------------------------------------------------------------------------
		// member functions
		void			Print() const;

		// scalar multiplication
		AEMtx44			Mult(float s) const;
		AEMtx44&		MultThis(float s);

		// matrix concatenation
		AEMtx44			Mult(const AEMtx44& rhs) const;
		AEMtx44&		MultThis(const AEMtx44& rhs);

		// vector multiplication
		AEVec3			MultVec(const AEVec3& v) const;
		AEVec3			MultVecSR(const AEVec3& v) const;
		AEVec4			MultVec(const AEVec4& v) const;

		// Transpose
		AEMtx44			Transpose() const;
		AEMtx44&		TransposeThis();

		// Extract the angles from the matrix
		void GetEulerAnglesXYZ(float & x, float & y, float & z) const;
		void GetEulerAnglesZYX(float & x, float & y, float & z) const;

		// the following functions will be applied to 'this' matrix
		AEMtx44&	ZeroThis();
		AEMtx44&	IdentityThis();
		AEMtx44&	ScaleThis(float x, float y, float z);
		AEMtx44&	RotateThis(float x, float y, float z);
		AEMtx44&	RotateThis(float x, float y, float z, float angle);
		AEMtx44&	TranslateThis(float x, float y, float z);

		// the following functions construct a matrix
		static AEMtx44	Zero();
		static AEMtx44	Identity();
		static AEMtx44	Scale(float x, float y, float z);
		static AEMtx44	Rotate(float x, float y, float z); // sames as default: Z-Y-X Euler
		static AEMtx44	RotateZYX(float x, float y, float z); // Z-Y-X Euler
		static AEMtx44	RotateXYZ(float x, float y, float z); // X-Y-Z Euler
		static AEMtx44	Rotate(float x, float y, float z, float angle);
		static AEMtx44	Translate(float x, float y, float z);
		static AEMtx44	ScaleInverse(float x, float y, float z);
		static AEMtx44	RotateInverse(float x, float y, float z); // sames as default: Z-Y-X Euler
		static AEMtx44	RotateZYXInverse(float x, float y, float z); // Z-Y-X Euler
		static AEMtx44	RotateXYZInverse(float x, float y, float z); // X-Y-Z Euler
		static AEMtx44	RotateInverse(float x, float y, float z, float angle);
		static AEMtx44	TranslateInverse(float x, float y, float z);

		// calculate a look at matrix
		// * camera is looking along -z axis and y is up
		static AEMtx44	LookAt(float posX, float posY, float posZ, float targetX, float targetY, float targetZ, float upX, float upY, float upZ);
		static AEMtx44	LookAt(const AEVec3& pos, const AEVec3& target, const AEVec3& up);
		static AEMtx44	InvLookAt(const AEVec3& pos, const AEVec3& target, const AEVec3& up);
		static AEMtx44	OrthoProj(float width, float height, float near=0.1f, float far=100.0f);
		static AEMtx44	OrthoProjGL(float width, float height, float near=0.1f, float far=100.0f);
		static AEMtx44	PerspProj(float fovY, float ratio, float near, float far);
		static AEMtx44	PerspProj(float fovY, float width, float height, float near, float far);
	};

	// ---------------------------------------------------------------------------

	template <typename T>
	AEMtx44 operator*(T s, const AEMtx44& rhs)
	{
		return (rhs * s);
	}
}
#pragma warning (default:4201)
// ---------------------------------------------------------------------------
#endif