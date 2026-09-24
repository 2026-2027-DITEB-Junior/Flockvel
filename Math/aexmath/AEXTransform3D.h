#ifndef AEX_TRANSFORM3D_H_
#define AEX_TRANSFORM3D_H_
#include "AEXMathDLL.h"
#include "AEXQuaternion.h"
#include "AEXMtx44.h"
namespace aex
{
	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------
	// CLASS : 3D Transform
	struct AEX_MATH_API Transform3D
	{
		Transform3D();
		Transform3D(const AEVec3 &pos, const AEVec3 &sca, const AEVec3 & eulers);
		Transform3D(const AEVec3 &pos, const AEVec3 &sca, const Quaternion & rot);
		Transform3D(const AEVec3 &pos, const AEVec3 &sca, const AEMtx33 &rot);

		AEVec3 position;
		AEVec3 scale;
		//AEVec3 angles;
		Quaternion rot;

		AEMtx44 GetMatrix() const;
		AEMtx44 GetInverseMatrix()const;

		// concatenates two transfomrs using same order as matrices
		Transform3D operator * (const Transform3D & rhs) const;
		Transform3D Concat(const Transform3D& rhs) const;
		Transform3D InvConcat(const Transform3D& rhs)const;
	};

}// namespace aex
// ---------------------------------------------------------------------------
#endif