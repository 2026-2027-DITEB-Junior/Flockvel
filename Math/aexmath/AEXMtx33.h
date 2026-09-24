#ifndef AE_MTX33_H
#define AE_MTX33_H

#include "AEXMathDLL.h"
#include "AEXVec2.h"

// ---------------------------------------------------------------------------
// Defines

#define AEMtx33RowCol(pMtx, row, col) (pMtx)->m[(row)][(col)]

namespace aex
{

	// ---------------------------------------------------------------------------
	// Struct/Class definitions
#pragma warning (disable:4201)

	// matrix is stored in row major (rows are stored consecutively in memory)
	struct AEX_MATH_API AEMtx33
	{
		union
		{
			float	m[3][3];
			float v[9];
			struct {
				float m11, m12, m13,
					m21, m22, m23,
					m31, m32, m33;
			};
		};


		// ------------------------------------------------------------------------
		// AEMtx33(): Default Constructor - Sets the matrix to identity.
		AEMtx33();

		// ------------------------------------------------------------------------
		// AEMtx33(): Custom Constructor - Sets the elements as the one specified.
		AEMtx33(float a11, float a12, float a13,
			float a21, float a22, float a23,
			float a31, float a32, float a33);

		// ------------------------------------------------------------------------
		// SetIdentity() - Set this matrix to identity
		void SetIdentity();

		// ------------------------------------------------------------------------
		// Transpose - return the transpose version of the matrix
		AEMtx33 Transpose()const;

		// ------------------------------------------------------------------------
		// TransposeThis - Sets this matrix as its transpose. 
		AEMtx33& TransposeThis();

		// ------------------------------------------------------------------------
		// Concat - Concatenate this matrix with 'rhs' such that result = this * rhs
		AEMtx33 Concat(const AEMtx33 & rhs)const;

		// ------------------------------------------------------------------------
		// operator * (AEMtx33) - Same as above but in operator form. 
		AEMtx33 operator *(const AEMtx33 & rhs)const;

		// ------------------------------------------------------------------------
		// operator *=(AEMtx33) - Concatenates this with 'rhs' as above and stores
		// the result in this matrix. Returns this matrix
		AEMtx33& operator *=(const AEMtx33 &rhs);

		// ------------------------------------------------------------------------
		// MultVec - Matrix to 2D Vector multiplication such that result = this * rhs
		AEVec2 MultVec(const AEVec2 & vec)const;

		// ------------------------------------------------------------------------
		// operator * - Matrix to 2D Vector multiplication such that result = this * rhs
		// same as above but in operator form.
		AEVec2 operator*(const AEVec2 & vec)const;

		// ------------------------------------------------------------------------
		// MultVecDir - Multiplies the vector by the matrix such that only the 
		// upper-left 2x2 matrix is considered (we assume we are multiplying a 
		// 2D vector and not a point.
		AEVec2 MultVecDir(const AEVec2 &vec)const;

		// ------------------------------------------------------------------------
		// Static Interface
		// ------------------------------------------------------------------------

		// ------------------------------------------------------------------------
		// AEMtx33::Identity - Returns a matrix as the identity
		static AEMtx33 Identity();

		// ------------------------------------------------------------------------
		// AEMtx33::Translate - Returns a translation matrix by a vector (x,y)
		static AEMtx33 Translate(float x, float y);

		// ------------------------------------------------------------------------
		// AEMtx33::Scale - Returns a scaling matrix by a factor (sx, sy)
		static AEMtx33 Scale(float sx, float sy);

		// ------------------------------------------------------------------------
		// AEMtx33::RotDeg - Returns a CCW rotation matrix by an angle specified 
		// in degrees. 
		static AEMtx33 RotDeg(float angle_deg);

		// ------------------------------------------------------------------------
		// AEMtx33::RotRad - Returns a CCW rotation matrix by an angle specified
		// in radians
		static AEMtx33 RotRad(float angle_rad);
	};
}
// ---------------------------------------------------------------------------

#pragma warning (default:4201)

#endif // MTX_H