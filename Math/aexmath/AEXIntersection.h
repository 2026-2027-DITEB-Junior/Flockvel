#ifndef AEX_COLLISIONS_H_
#define AEX_COLLISIONS_H_

#include "AEXMath.h"

namespace aex
{
	struct AEX_MATH_API Rect2D
	{
		Rect2D(AEVec2 pos, AEVec2 s) {
			p.x = pos.x;
			p.y = pos.y;

			size.x = s.x;
			size.y = s.y;
		}
		aex::AEVec2 p;
		aex::AEVec2 size;
		bool Contains(const AEVec2 &pt)const;
	};
	struct AEX_MATH_API Line2D
	{
		AEVec2 start, end;
		inline AEVec2 GetNormal() { return (end - start).Perp().Normalize(); }
	};
	struct AEX_MATH_API Ray3D 
	{
		AEVec3 mOrigin;
		AEVec3 mDirection;
	};
	struct AEX_MATH_API Contact2D
	{
		AEVec2	mIntersectionPt;
		AEVec2	mNormal;
		float	mPenetration;

	};

	// ------------------------------------------------------------------------
	// Intersection FUNCTIONS
	// ------------------------------------------------------------------------

	// PointInRect
	bool AEX_MATH_API PointInRect(const AEVec2 &pt, const AEVec2& rect_pos, const AEVec2 &rect_size);

	// PointInCircle
	bool AEX_MATH_API PointInCircle(const AEVec2& pt, const AEVec2& circle_pos, float radius);

	bool AEX_MATH_API PointInOBB(const AEVec2& pt, const Transform& OBB);


	// ------------------------------------------------------------------------
	// PointToLine
	float AEX_MATH_API PointToLine(AEVec2 point, AEVec2 start_line, AEVec2 end_line);

	// ------------------------------------------------------------------------
	// TestRecttoLine
	bool AEX_MATH_API TestRecttoLine(Rect2D box, Line2D line);

	// ------------------------------------------------------------------------
	// RectToRect
	bool AEX_MATH_API RectToRect(const Rect2D& a, const Rect2D& b, Contact2D* outContact = nullptr);

	// ------------------------------------------------------------------------
	// CircleToCircle
	bool AEX_MATH_API CircleToCircle(const AEVec2& c1, float r1, const AEVec2& c2, float r2, Contact2D* outContact);

	// ------------------------------------------------------------------------
	// RectToCircle
	bool AEX_MATH_API RectToCircle(const Rect2D &rect, const AEVec2 &circlePos, float radius, Contact2D* outContact = nullptr);

	// ------------------------------------------------------------------------
	// OBBToCircle
	bool AEX_MATH_API OBBToCircle(const Transform& OBB, const AEVec2& circlePos, float radius, Contact2D* outContact = nullptr);

	// ------------------------------------------------------------------------
	// OBBToOBB
	bool AEX_MATH_API OBBToOBB(const Transform& OBB1, const Transform& OBB2, Contact2D* outContact = nullptr, bool findPt = false);

	// ------------------------------------------------------------------------
	// RayToZPlane
	float AEX_MATH_API RayToZPlane(const Ray3D& ray, float z = 0.0f);
	float AEX_MATH_API RayToZPlane(const AEVec3 &r, const AEVec3 &d, float z = 0.0f);

	// ------------------------------------------------------------------------
	// RayToAABB
	float AEX_MATH_API RayToAABB(const Ray3D& ray, const AEVec3& minP, const AEVec3& maxP);
	float AEX_MATH_API RayToAABB(const AEVec3 &r, const AEVec3 &d, const AEVec3 &minP, const AEVec3 &maxP);

	// ------------------------------------------------------------------------
	// RayToOBB
	float AEX_MATH_API RayToOBB(const Ray3D& ray, const Transform3D& obb);
	float AEX_MATH_API RayToOBB(const AEVec3 &r, const AEVec3 &d, const Transform3D &obb);

	// ------------------------------------------------------------------------
	// Collision Resolution
	void AEX_MATH_API ResolveContactVelocities(AEVec2& vel1, AEVec2& vel2, const Contact2D& contact, float invMass1, float invMass2, float restitution);

	//Collision Resolution
	void AEX_MATH_API AEResolveContactPenetration(AEVec2& pos1, AEVec2& pos2, const Contact2D& contact, float invMass1, float invMass2);

} // namespace aex

// ----------------------------------------------------------------------------
#endif