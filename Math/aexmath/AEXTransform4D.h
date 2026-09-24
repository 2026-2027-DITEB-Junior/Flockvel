#ifndef AEX_TRANSFORM4D_H_
#define AEX_TRANSFORM4D_H_
#include "AEXMathDLL.h"
#include "AEXQuaternion.h"
#include "AEXMtx44.h"
#include "AEXVec4.h"
#include "AEXTransform3D.h"
namespace aex
{
	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------
	// CLASS : 4D Transform
	struct AEX_MATH_API Transform4D
	{
		Transform4D();
		Transform4D(const Transform3D& trs);
		Transform4D(const AEVec3& pos, const AEVec3& sca, const AEVec3& eulers);
		Transform4D(const AEVec3& pos, const AEVec3& sca, const Quaternion& rot);
		Transform4D(const AEVec3& pos, const AEVec3& sca, const AEMtx33& rot);

		AEVec4 position;
		AEVec4 scale;
		//AEVec3 angles;
		Quaternion rot;

		AEMtx44 GetMatrix() const;
		AEMtx44 GetInverseMatrix()const;

		// concatenates two transfomrs using same order as matrices
		Transform4D operator * (const Transform3D& rhs) const;
		Transform4D Concat(const Transform3D& rhs) const;
		Transform4D InvConcat(const Transform3D& rhs)const;
	};

}// namespace aex
// ---------------------------------------------------------------------------
#endif