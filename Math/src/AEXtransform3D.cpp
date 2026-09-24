// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	Transform3D.h
// Author			:	Thomas Komair
// Creation Date	:	2013/04/26
// Purpose			:	Basic 3D transformation (scale-rotate-translate)
// History			:
// ---------------------------------------------------------------------------
#include "AEXMath.h"

namespace aex
{
	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------
	// CLASS : 3D TRansform

	Transform3D::Transform3D()
		: position(0, 0, 0)
		, scale(1, 1, 1)
		, rot(0, 0, 0, 1)
	{}
	Transform3D::Transform3D(const AEVec3 &pos, const AEVec3 &sca, const AEVec3 & eulers)
		: position(pos)
		, scale(sca)
	{
		rot = Quaternion::MakeEulerXYZ(eulers.x, eulers.y, eulers.z);
	}
	Transform3D::Transform3D(const AEVec3 &pos, const AEVec3 &sca, const Quaternion & _rot)
		: position(pos)
		, scale(sca)
		, rot(_rot)
	{}
	Transform3D::Transform3D(const AEVec3 &pos, const AEVec3 &sca, const AEMtx33 &_rot)
		: position(pos)
		, scale(sca)
	{
		rot = Quaternion::MakeMatrix33(_rot);
	}

	AEMtx44 Transform3D::GetMatrix() const
	{
		AEMtx44 s, r, t;
		s = AEMtx44::Scale(scale.x, scale.y, scale.z);
		r = rot.ToMtx44();
		t = AEMtx44::Translate(position.x, position.y, position.z);

		// compute the final transformation matrix.
		return t * r * s;
	}

	AEMtx44 Transform3D::GetInverseMatrix() const
	{
		AEMtx44 invS, invR, invT;
		float invSX = FLOAT_ZERO(scale.x) ? 0.0f : 1.0f / scale.x;
		float invSY = FLOAT_ZERO(scale.y) ? 0.0f : 1.0f / scale.y;
		float invSZ = FLOAT_ZERO(scale.z) ? 0.0f : 1.0f / scale.z;
		invS = AEMtx44::Scale(invSX, invSY, invSZ);
		invR = rot.Inverse().ToMtx44();
		invT = AEMtx44::Translate(-position.x, -position.y, -position.z);

		// compute final matrix
		return invS * invR * invT;
	}

	// concatenates two transfomrs using same order as matrices
	Transform3D Transform3D::operator * (const Transform3D & rhs)const
	{
		Transform3D res;

		// scale gets MULTIPLIED
		res.scale = AEVec3(this->scale.x * rhs.scale.x
			, this->scale.y * rhs.scale.y
			, this->scale.z * rhs.scale.z);

		// rotate is combined
		res.rot = rot * rhs.rot;

		// translation is affected by the rotation and scale 
		AEVec3 temp = rhs.position;
		temp.x *= scale.x;
		temp.y *= scale.y;
		temp.z *= scale.z;

		// 
		auto tmpQ = rot;
		//tmpQ.w = -tmpQ.w;
		res.position = position + tmpQ * temp;

		return res;
	}
	Transform3D Transform3D::Concat(const Transform3D& rhs)const
	{
		return this->operator*(rhs);
	}
	Transform3D Transform3D::InvConcat(const Transform3D& rhs)const
	{
		Transform3D res;
		res.scale = {
			FLOAT_ZERO(this->scale.x) ? rhs.scale.x : rhs.scale.x / this->scale.x,
			FLOAT_ZERO(this->scale.y) ? rhs.scale.y : rhs.scale.y / this->scale.y,
			FLOAT_ZERO(this->scale.z) ? rhs.scale.z : rhs.scale.z / this->scale.z
		};

		res.rot = rot.Inverse() * rhs.rot;

		res.position = rot.Inverse() * (rhs.position - position);
		res.position = {
			FLOAT_ZERO(this->scale.x) ? res.position.x : res.position.x / this->scale.x,
			FLOAT_ZERO(this->scale.y) ? res.position.y : res.position.y / this->scale.y,
			FLOAT_ZERO(this->scale.z) ? res.position.z : res.position.z / this->scale.z
		};

		return res;
	}
}