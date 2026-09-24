#include "AEXMath.h"

namespace aex
{
	// ------------------------------------------------------------------------
	Transform::Transform()
		: mTranslation(0,0,0)
		, mScale(AEVec2(1,1))
		, mOrientation(0.0f)
	{}
	// ------------------------------------------------------------------------
	Transform::Transform(const AEVec3 & pos, const AEVec2 & scale, float rot)
		: mTranslation(pos)
		, mScale(scale)
		, mOrientation(rot)
	{
	}
	// ------------------------------------------------------------------------
	AEMtx33 Transform::GetMatrix()const
	{
		return AEMtx33::Translate(mTranslation.x, mTranslation.y) * AEMtx33::RotRad(mOrientation) * AEMtx33::Scale(mScale.x, mScale.y);
	}

	// ------------------------------------------------------------------------
	AEMtx33 Transform::GetInvMatrix()const
	{
		AEMtx33 scale_mtx = AEMtx33::Scale(	FLOAT_ZERO(mScale.x) ? 0.0f : 1.0f / mScale.x,
											FLOAT_ZERO(mScale.y) ? 0.0f : 1.0f / mScale.y);
		AEMtx33 rot_mtx = AEMtx33::RotRad(-mOrientation);
		AEMtx33 trans_mtx = AEMtx33::Translate(-mTranslation.x, -mTranslation.y);
		return scale_mtx * rot_mtx * trans_mtx;
	}

	// ------------------------------------------------------------------------
	AEMtx44 Transform::GetMatrix44()const
	{
		AEMtx44 scale_mtx = AEMtx44::Scale(mScale.x, mScale.y, 1.0f);
		AEMtx44 rot_mtx = AEMtx44::RotateXYZ(0, 0, mOrientation);
		AEMtx44 trans_mtx = AEMtx44::Translate(mTranslation.x, mTranslation.y, mTranslation.z);
		return (trans_mtx * rot_mtx * scale_mtx);
	}

	// ------------------------------------------------------------------------
	AEMtx44 Transform::GetInvMatrix44()const
	{
		AEMtx44 scale_mtx = AEMtx44::Scale(	FLOAT_ZERO(mScale.x) ? 0.0f : 1.0f/mScale.x,
											FLOAT_ZERO(mScale.y) ? 0.0f : 1.0f/mScale.y, 1.0f);
		AEMtx44 rot_mtx = AEMtx44::RotateXYZ(0, 0, -mOrientation);
		AEMtx44 trans_mtx = AEMtx44::Translate(-mTranslation.x, -mTranslation.y, -mTranslation.z);
		return scale_mtx * rot_mtx * trans_mtx;
	}

	// ------------------------------------------------------------------------
	Transform Transform::Concat(const Transform &rhs)const
	{
		Transform rh = rhs;
		Transform res; 
		AEVec2 resTranslation, resScale;
		float	resRot;

		float sinA = sin(mOrientation);
		float cosA = cos(mOrientation);

		float sX = mScale.x;
		float sY = mScale.y;

		// scale
		resScale.x = mScale.x * rhs.mScale.x;
		resScale.y = mScale.y * rhs.mScale.y;

		// rotation
		resRot = mOrientation + rhs.mOrientation;

		// translate
		resTranslation.x = mTranslation.x + cosA * sX * rhs.mTranslation.x - sinA * sY * rhs.mTranslation.y;
		resTranslation.y = mTranslation.y + sinA * sX * rhs.mTranslation.x + cosA * sY * rhs.mTranslation.y;

		// store results.
		res.mTranslation = AEVec3(resTranslation.x, resTranslation.y, rhs.mTranslation.z + mTranslation.z);
		res.mScale = resScale;
		res.mOrientation = resRot;
		return res;
	}
	Transform Transform::InverseConcat(const Transform& rhs) const
	{
		Transform res;
		res.mScale.x = rhs.mScale.x / this->mScale.x;
		res.mScale.y = rhs.mScale.y / this->mScale.y;
		res.mOrientation = rhs.mOrientation - this->mOrientation;
		res.mTranslation = AEMtx33::Scale(1.0f / this->mScale.x, 1.0f / this->mScale.y)
			* AEMtx33::RotRad(-this->mOrientation) * 
			AEVec2(rhs.mTranslation- this->mTranslation);
		return res;
	}
	// ------------------------------------------------------------------------
	Transform Transform::operator *(const Transform &rhs)const
	{
		return Concat(rhs);
	}
	Transform& Transform::operator*=(const Transform& rhs)
	{
		return *this = *this * rhs;
	}
	// ------------------------------------------------------------------------
	AEVec2 Transform::MultVec(const AEVec2 &rhs)const
	{
		return GetMatrix() * rhs;
	}
	AEVec2 Transform::InvMultVec(const AEVec2& rhs)const
	{
		return AEMtx33::Scale(1.0f / this->mScale.x, 1.0f / this->mScale.y) * 
			AEMtx33::RotRad(-this->mOrientation) * (rhs - this->mTranslation);
	}
	AEVec3 Transform::InvMultVec(const AEVec3& rhs)const
	{
		return AEMtx44(AEMtx33::Scale(1.0f / this->mScale.x, 1.0f / this->mScale.y)) *
			AEMtx44(AEMtx33::RotRad(-this->mOrientation)) * (rhs - this->mTranslation);
	}
	// ------------------------------------------------------------------------
	AEVec2 Transform::operator *(const AEVec2 & rhs)const
	{
		return MultVec(rhs);
	}
}