//-----------------------------------------------------------------------------
#ifndef AEX_TRANSFORM_H_
#define AEX_TRANSFORM_H_
//-----------------------------------------------------------------------------
#include "AEXMathDLL.h"

namespace aex
{
	struct AEX_MATH_API Transform
	{
		Transform();
		Transform(const AEVec3 & pos, const AEVec2 & scale, float rot);

		AEVec2		mScale;				// X-Y Scale
		AEVec3		mTranslation;		// Position (x,y,z) for rendering ignored for 3x3 matrix
		float		mOrientation;		// Angle of Rotation

		// wrapper functions around globals;
		AEMtx33		GetMatrix()const;
		AEMtx33		GetInvMatrix()const;
		AEMtx44		GetMatrix44()const;
		AEMtx44		GetInvMatrix44()const;

		Transform	Concat(const Transform &rhs)const;
		Transform	InverseConcat(const Transform& rhs)const;
		AEVec2		MultVec(const AEVec2 &rhs)const;
		AEVec2		InvMultVec(const AEVec2& rhs)const;
		AEVec3		InvMultVec(const AEVec3& rhs)const;
		AEVec2		operator *(const AEVec2 & rhs)const;
		Transform	operator *(const Transform &rhs)const;
		Transform&	operator *=(const Transform& rhs);
	};
}
#endif // TRANSFORM_H_