#include "AEXIntersection.h"
namespace aex
{

	// ------------------------------------------------------------------------
	// Intersection FUNCTIONS
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	// Rect2D Point containment
	bool Rect2D::Contains(const AEVec2 &pt)const
	{
		AEVec2 hs = size / 2.0f;
		float l = p.x - hs.x;
		float r = p.x + hs.x;
		float b = p.y - hs.y;
		float t = p.y + hs.y;
		return (l <= pt.x && pt.x <= r && b <= pt.y && pt.y <= t);
	}
	bool PointInRect(const AEVec2 &pt, const AEVec2 &rect_pos, const AEVec2 &rect_size)
	{
		AEVec2 hs = rect_size / 2.0f;
		float l = rect_pos.x - hs.x;
		float r = rect_pos.x + hs.x;
		float b = rect_pos.y - hs.y;
		float t = rect_pos.y + hs.y;
		return (l <= pt.x && pt.x <= r && b <= pt.y && pt.y <= t);
	}

	bool PointInCircle(const AEVec2& pt, const AEVec2 &circle_pos, float radius)
	{
		// compute the distance between the point P and the center
		float dist_sq = pt.DistanceSq(circle_pos);

		// return wether the point is inside the 
		return(dist_sq <= radius * radius);
	}

	bool PointInOBB(const AEVec2& pt, const Transform& OBB)
	{	
		// compute the inverse rotation matrix and apply to point
		AEVec2 localPt = AEMtx33::Translate(-OBB.mTranslation.x, -OBB.mTranslation.y) * pt;
		localPt = AEMtx33::RotRad(-OBB.mOrientation) * localPt;
		Rect2D localRect = { {0.0f,0.0f}, OBB.mScale };
		return localRect.Contains(localPt);
	}

	// ------------------------------------------------------------------------
	// PointToLine
	float PointToLine(AEVec2 point, AEVec2 start_line, AEVec2 end_line)
	{
		// compute edge vector
		AEVec2 e = end_line - start_line;
		// compute normal
		AEVec2 n = e.Perp();
		// compute ndot v
		float ndotv = n.Dot(start_line);
		// compute ndot p
		float ndotp = n.Dot(point);
		// compute the side of the 
		return ndotp - ndotv;
	}

	// ------------------------------------------------------------------------
	// TestRecttoLine
	bool TestRecttoLine(Rect2D box, Line2D line)
	{
		float hw = box.size.x / 2.0f;
		float hh = box.size.y / 2.0f;

		// compute all the corners of thebox
		AEVec2 corners[4] = {
			box.p + aex::AEVec2(-hw, hh),
			box.p + aex::AEVec2(hw, hh),
			box.p + aex::AEVec2(-hw, -hh),
			box.p + aex::AEVec2(hw, -hh)
		};

		//compute edges vector (both direction)
		AEVec2 edges[2] = { line.end - line.start, line.start - line.end };


		// trivial rejection test if all corners are located outside of the line segment
		unsigned int corner_inside = 0;
		for (int i = 0; i < 4; ++i)
		{
			// get the vector from each line vertex
			AEVec2 c_to_v0 = corners[i] - line.start;
			AEVec2 c_to_v1 = corners[i] - line.end;

			// check the dot product to determine if the corner is inside
			corner_inside += (c_to_v0.Dot(edges[1]) < 0 && c_to_v1.Dot(edges[0]) < 0) ? 1 : 0;
		}

		if (corner_inside == 0)
			return false;

		// vector from center of boxto first vertex
		AEVec2 distVec = box.p - line.start;

		// trivial rejection: distance
		float distSq = distVec.ProjectPerp(edges[0]).LengthSq();

		// box radius
		float radiusSq = hw * hw + hh * hh;

		if (distSq > radiusSq)
			return false;

		// get all the corners of the box and test that they are all in the same side. 
		for (int i = 0; i < 4; ++i)
		{
			float inside = PointToLine(corners[i], line.start, line.end);
			if (inside < 0) // inside
				// there's and intersection
				return true;
		}
		return false;
	}

	// ------------------------------------------------------------------------
	// RectToRect
	bool RectToRect(const Rect2D& a, const Rect2D& b, Contact2D* outContact)
	{
		// compute the rect sum
		AEVec2 sum_size = a.size + b.size;
		Rect2D tmp = { a.p, sum_size };

		bool contained = tmp.Contains(b.p);
		const AEVec2& pos1 = a.p;
		const AEVec2& pos2 = b.p;

		// if collision data is requested, add the necessary data
		if (contained && outContact != nullptr) {

			// get distance vector
			AEVec2 dist = pos2 - pos1;
			AEVec2 half_size_sum = sum_size * 0.5f;

			// project dist onto aabb axes and compute penetration on each axis
			float penX = half_size_sum.x - dist.Project(AEVec2(1, 0)).Length();
			float penY = half_size_sum.y - dist.Project(AEVec2(0, 1)).Length();

			// compute normal & penetration
			outContact->mNormal = AEVec2(1, 0);
			outContact->mPenetration = penX;

			// check on which axis is the maximum penetration 
			// note: here we check for the lesser value because it is computed
			// above as the difference of the radius_sum and the projection length
			// so the smaller the value the closer the two boxes.
			if (penY < penX)
			{
				outContact->mNormal = AEVec2(0, 1);
				outContact->mPenetration = penY;
			}

			// make sure that the normal is oriented correctly (convention is rect1 to rect2)
			if (outContact->mNormal.Dot(dist) < 0)
				outContact->mNormal = -outContact->mNormal;

			// generate corners. 
			AEVec2 incident_corners[4];
			incident_corners[0] = pos2 + AEVec2(-b.size.x, +b.size.y) * 0.5f;
			incident_corners[1] = pos2 + AEVec2(-b.size.x, -b.size.y) * 0.5f;
			incident_corners[2] = pos2 + AEVec2(b.size.x, -b.size.y) * 0.5f;
			incident_corners[3] = pos2 + AEVec2(b.size.x, +b.size.y) * 0.5f;

			outContact->mIntersectionPt = incident_corners[0];
			for (int i = 0; i < 4; ++i)
			{
				//if (dir.)
				if (a.Contains(incident_corners[i]))
				{
					outContact->mIntersectionPt = incident_corners[i];
					return true;
				}
			}

			incident_corners[0] = pos1 + AEVec2(-a.size.x, +a.size.y) * 0.5f;
			incident_corners[1] = pos1 + AEVec2(-a.size.x, -a.size.y) * 0.5f;
			incident_corners[2] = pos1 + AEVec2(a.size.x, -a.size.y) * 0.5f;
			incident_corners[3] = pos1 + AEVec2(a.size.x, +a.size.y) * 0.5f;

			outContact->mIntersectionPt = incident_corners[0];
			for (int i = 0; i < 4; ++i)
			{
				//if (dir.)
				if (b.Contains(incident_corners[i]))
				{
					outContact->mIntersectionPt = incident_corners[i];
					return true;
				}
			}
		}

		return contained;
	}

	// ------------------------------------------------------------------------
	// CircleToCircle
	bool CircleToCircle(const AEVec2& c1, float r1, const AEVec2& c2, float r2, Contact2D* outContact)
	{

		float radiusSum = r1 + r2;
		float CentersDistanceSquare = c1.DistanceSq(c2);
		bool contained = CentersDistanceSquare <= radiusSum * radiusSum;

		if (contained && outContact != nullptr) {
			outContact->mNormal = c2-c1;// compute normal
			outContact->mPenetration = radiusSum - outContact->mNormal.Length();
			outContact->mNormal.NormalizeThis();
			outContact->mIntersectionPt = c1 + outContact->mNormal * r1;
		}

		return contained;
	}

	// ------------------------------------------------------------------------
	// RectToCircle
	bool RectToCircle(const Rect2D& rect, const AEVec2& circlePos, float radius, Contact2D* outContact)
	{
		float top = rect.p.y + rect.size.y * .5f;
		float bottom = rect.p.y - rect.size.y * .5f;
		float left = rect.p.x - rect.size.x * .5f;
		float right = rect.p.x + rect.size.x * .5f;

		// method 1: clamp the circle center to the rectangle and check distance
		AEVec2 nearestPt;
		nearestPt.x = fmaxf(left, fminf(right, circlePos.x));
		nearestPt.y = fmaxf(bottom, fminf(top, circlePos.y));

		// if that point is inside the circle->collision
		if (!PointInCircle(nearestPt, circlePos, radius))
			return false;

		if (outContact)
		{
			outContact->mIntersectionPt = nearestPt;

			// case 1: center is contained in box -> nearestPt == Center
			if (PointInRect(circlePos, rect.p, rect.size))
			{
				// easier solution: project the center of the circle onto the box's 
				// axes, pick the axis with smaller penetration and push the circle out. 
				AEVec2 D = circlePos - rect.p;
				float penX = rect.size.x / 2.0f - fabs(D.x);
				float penY = rect.size.y / 2.0f - fabs(D.y);
				outContact->mNormal = penX < penY ? AEVec2(1, 0) : AEVec2(0, 1);
				if (D.Dot(outContact->mNormal) < 0)
					outContact->mNormal = -outContact->mNormal;

				outContact->mPenetration = penX < penY ? penX : penY;
				outContact->mPenetration += radius;

				outContact->mIntersectionPt = circlePos;
			}

			else // case 2: center is not contained in box -> nearestPt != circlePos
			{
				outContact->mIntersectionPt = nearestPt;
				outContact->mNormal = circlePos - nearestPt;	// should be normalized
				outContact->mPenetration = radius - outContact->mNormal.Length(); // trick use normal to avoid computing D vector again.
				outContact->mNormal.NormalizeThis(); // now normalize
			}
		}

		// return overlap
		return true;
	}

	// ------------------------------------------------------------------------
	// OBBToCircle
	bool OBBToCircle(const Transform& OBB, const AEVec2& circlePos, float radius, Contact2D* outContact)
	{
		// rotate both the circle and the obb so obb is aabb
		AEMtx33 invRotMtx = AEMtx33::RotRad(-OBB.mOrientation);
		AEVec2 localCenter = invRotMtx * circlePos;
		AEVec2 localRectPos = invRotMtx * AEVec2(OBB.mTranslation.x, OBB.mTranslation.y);
		Rect2D localRect = { localRectPos, OBB.mScale };
		if (RectToCircle(localRect, localCenter, radius, outContact))
		{
			// transform back the results
			AEMtx33 rotMtx = AEMtx33::RotRad(OBB.mOrientation);
			outContact->mIntersectionPt = rotMtx * outContact->mIntersectionPt;
			outContact->mNormal = rotMtx * outContact->mNormal;
			return true;
		}
		return false;
	}

	// ------------------------------------------------------------------------
	// OBBToOBB
	bool OBBToOBB(const Transform& OBB1, const Transform& OBB2, Contact2D* outContact, bool findPt)
	{
		// find the axes of each box (0 -> X, 1 -> Y)
		AEVec2 axes[4], halfEdges[4];
		float halfScales[4];

		// set the half scales for later processing
		halfScales[0] = OBB1.mScale.x / 2.0f;
		halfScales[1] = OBB1.mScale.y / 2.0f;
		halfScales[2] = OBB2.mScale.x / 2.0f;
		halfScales[3] = OBB2.mScale.y / 2.0f;

		// Get the X axis from the angle of orientation of each obb
		axes[0].FromAngle(OBB1.mOrientation);
		axes[2].FromAngle(OBB2.mOrientation);

		// Y axes is perpendicular to the x axes for each obb
		axes[1] = axes[0].Perp();
		axes[3] = axes[2].Perp();

		// compute the half edges for each box
		for (unsigned int i = 0; i < 4; ++i)
			halfEdges[i] = axes[i] * halfScales[i];

		// get the distance between each box
		AEVec2 distVec = OBB2.mTranslation - OBB1.mTranslation;
		float	dist = distVec.Length();

		// now that we have the axes, we can project each of the vertices
		// of the boxes onto the other. If we find a separating axis then 
		// there is no intersection
		float projectLength[4];
		float projectD[4];
		unsigned int minAxis = UINT_MAX;
		float minVal = FLT_MAX;
		for (unsigned int i = 0; i < 4; ++i)
		{
			// projection of the distVec onto axis
			projectD[i] = fabs(distVec.Dot(axes[i]));

			projectLength[i] = 0.0f;
			// compute the projection for each of the hald edges
			for (int j = 0; j < 4; ++j)
			{
				if (i == j)
					projectLength[i] += halfScales[j];
				else
					projectLength[i] += fabs(halfEdges[j].Dot(axes[i]));
			}

			float penetration = projectD[i] - projectLength[i];
			if (penetration > 0)
				return false;

			penetration = fabsf(penetration);
			if (penetration < minVal)
			{
				minVal = penetration;
				minAxis = i;
				if (outContact)
					outContact->mPenetration = penetration;
			}
		}
		// there is no separating axis, there is an interesection

		// TODO compute contact data
		if (outContact)
		{
			// 
			// Determine Normal
			//

			// by default, the normal is the perpendicular axis 
			// to the minimum overlapping axis, i.e the other one
			unsigned int nIdx = minAxis;
			outContact->mNormal = axes[nIdx];

			// now depending on the relative position of the boxes, flip the normal
			if (outContact->mNormal.Dot(distVec) < 0)
				outContact->mNormal *= -1.0f;
			//
			// Determine the point of intersection. 
			// 
			if (findPt) 
			{
				unsigned int offset = 0;
				AEVec2 pos = { OBB1.mTranslation.x, OBB1.mTranslation.y };
				AEVec2 otherpos = { OBB2.mTranslation.x, OBB2.mTranslation.y };
				unsigned int other_offset = (offset + 2) % 4;
				if (minAxis >= 2)// vertex comes from OBB2 & normal comes from OBB1
				{
					offset = 2;
					other_offset = (offset + 2) % 4;

					// swap positions
					std::swap(pos, otherpos);
				}

				float xdotT = distVec.Dot(axes[offset]);
				float ydotT = distVec.Dot(axes[offset + 1]);


				// TODO: Clip the vertex using sotherland hodgeman. 
				// generate corners. 
				AEVec2 incident_corners[4];
				incident_corners[0] = otherpos + halfEdges[other_offset] + halfEdges[other_offset + 1];
				incident_corners[1] = otherpos - halfEdges[other_offset] + halfEdges[other_offset + 1];
				incident_corners[2] = otherpos - halfEdges[other_offset] - halfEdges[other_offset + 1];
				incident_corners[3] = otherpos + halfEdges[other_offset] - halfEdges[other_offset + 1];

				// check the incident corners with the obb where the normal is coming from
				for (int i = 0; i < 4; ++i)
				{
					const Transform * obb_to_test = offset == 0 ? &OBB1 : &OBB2;
					if (PointInOBB(incident_corners[i], *obb_to_test))
					{
						outContact->mIntersectionPt = incident_corners[i];
						break;
					}
				}
			}
		}

		return true;
	}

	// ------------------------------------------------------------------------
	// RayToZPlane
	float RayToZPlane(const Ray3D& ray, float z)
	{
		return RayToZPlane(ray.mOrigin, ray.mDirection, z);
	}
	float RayToZPlane(const AEVec3& r, const AEVec3& d, float z) {
		auto n = AEVec3(0, 0, 1);
		auto v0 = AEVec3(0, 0, z);
		auto ndotv0 = n * v0;
		auto ndotr = n * r;
		auto ndotd = n * d;
		float t = (ndotv0 - ndotr) / ndotd;
		return t;
	}

	// ------------------------------------------------------------------------
	// RayToAABB
	float RayToAABB(const Ray3D& ray, const AEVec3& minP, const AEVec3& maxP)
	{
		return RayToAABB(ray.mOrigin, ray.mDirection, minP, maxP);
	}
	float RayToAABB(const AEVec3& r, const AEVec3& d, const AEVec3& minP, const AEVec3& maxP)
	{
		float maxS = 0.0f;
		float minT = FLT_MAX;

		// do x coordinate test (yz planes)
		{
			float s, t;
			float recipX = 1.0f / d.x;
			if (recipX >= 0.0f) {
				s = (minP.x - r.x) * recipX;
				t = (maxP.x - r.x) * recipX;
			}
			else {
				s = (maxP.x - r.x) * recipX;
				t = (minP.x - r.x) * recipX;
			}

			// adjust min and max values
			if (s > maxS) maxS = s;
			if (t < minT) minT = t;

			if (maxS > minT)
				return -1.0f;
		}

		// do coordinate test (xz planes)
		{
			float s, t;
			float recipY = 1.0f / d.y;
			if (recipY >= 0.0f) {
				s = (minP.y - r.y) * recipY;
				t = (maxP.y - r.y) * recipY;
			}
			else {
				s = (maxP.y - r.y) * recipY;
				t = (minP.y - r.y) * recipY;
			}

			// adjust min and max values
			if (s > maxS) maxS = s;
			if (t < minT) minT = t;

			if (maxS > minT)
				return -1.0f;
		}

		// do coordinate test (xy planes)
		{
			float s, t;
			float recipZ = 1.0f / d.z;
			if (recipZ >= 0.0f) {
				s = (minP.z - r.z) * recipZ;
				t = (maxP.z - r.z) * recipZ;
			}
			else {
				s = (maxP.z - r.z) * recipZ;
				t = (minP.z - r.z) * recipZ;
			}

			// adjust min and max values
			if (s > maxS) maxS = s;
			if (t < minT) minT = t;

			if (maxS > minT)
				return -1.0f;
		}

		return maxS;
	}

	// ------------------------------------------------------------------------
	// RayToOBB
	float RayToOBB(const Ray3D& ray, const Transform3D& obb)
	{
		return RayToOBB(ray.mOrigin, ray.mDirection, obb);
	}
	float RayToOBB(const AEVec3& r, const AEVec3& d, const Transform3D& obb) {

		// tranform ray and origin to space of obb
		auto invTr = obb.GetInverseMatrix();
		auto rt = invTr.MultVec(r);
		auto dt = invTr.MultVecSR(d);

		// compute AABB extents
		//auto minP = obb.scale * -0.5f;
		//auto maxP = obb.scale * 0.5f;
		AEVec3 minP = { -0.5f, -.5f, -.5f };
		AEVec3 maxP = { .5f, .5f, .5f };

		// do ray test with ABB
		float t = RayToAABB(rt, dt, minP, maxP);

		// return intersection
		return t;
	}

	// ------------------------------------------------------------------------
	// Collision Resolution
	void ResolveContactVelocities(AEVec2& vel1, AEVec2& vel2, const Contact2D& contact, float invMass1, float invMass2, float restitution)
	{
		// resolve
		float totalInvMass = invMass1 + invMass2;

		// compute the mass influence
		float massInfluence1 = (invMass1 / totalInvMass);
		float massInfluence2 = (invMass2 / totalInvMass);

		// compute relative distance and velocity from object 1 to object 2
		AEVec2 rel_vel = vel2 - vel1;

		// get separating velocity (as defined in Millington. pp114).
		float sep_vel = rel_vel.Dot(contact.mNormal);
		if (sep_vel < 0)
		{
			float new_sep_vel = -sep_vel * restitution;
			float delta_sep_vel = new_sep_vel - sep_vel;

			// apply to velocities
			vel1 -= contact.mNormal * massInfluence1 * massInfluence1 * delta_sep_vel;
			vel2 += contact.mNormal * massInfluence2 * massInfluence2 * delta_sep_vel;
		}

	}
	//Collision Resolution
	void AEResolveContactPenetration(AEVec2& pos1, AEVec2& pos2, const Contact2D& contact, float invMass1, float invMass2)
	{

		// resolve
		float totalInvMass = invMass1 + invMass2;

		// compute the mass influence
		float massInfluence1 = (invMass1 / totalInvMass);
		float massInfluence2 = (invMass2 / totalInvMass);

		// apply movement
		pos1 -= contact.mNormal * contact.mPenetration * massInfluence1;
		pos2 += contact.mNormal * contact.mPenetration * massInfluence2;
	}
}