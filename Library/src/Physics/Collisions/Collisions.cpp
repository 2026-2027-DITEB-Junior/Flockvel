#include "pch.h"
#include "Collisions.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <algorithm>
#include <array>

namespace clc {

    // Sphere vs sphere collision
    bool SphereToSphere(SphereRb* a, SphereRb* b, Contact& contact) {
        glm::vec3 posA = glm::vec3(a->mTransform->mPosition);
        glm::vec3 posB = glm::vec3(b->mTransform->mPosition);
        glm::vec3 normal = posB - posA;
        float distSq = glm::length2(normal);
        float radiusSum = a->radius + b->radius;

        // No collision
        if (distSq >= radiusSum * radiusSum) return false;

        contact.bodyA = a;
        contact.bodyB = b;
        contact.contactCount = 1;

        // In case the overlap is almost perfect
        if (distSq <= 0.000001f) {
            contact.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            contact.points[0].penetration = a->radius;
            contact.points[0].position = posA;
        }
        // Calculate point for most cases
        else {
            float invDist = glm::inversesqrt(distSq);
            float distance = distSq * invDist;
            contact.normal = normal * invDist;
            contact.points[0].penetration = radiusSum - distance;
            contact.points[0].position = posA + (contact.normal * a->radius);
        }

        contact.points[0].normalImpulseSum = 0.0f;
        contact.points[0].tangentImpulseSum1 = 0.0f;
        contact.points[0].tangentImpulseSum2 = 0.0f;
        return true;
    }

    // Helper for SAT, checks the axis and the edge crosses
    bool TestAxis(const BoxRb& a, const BoxRb& b, glm::vec3 axis, glm::vec3& dist, float& minPenetration, glm::vec3& bestAxis, int axisIndex, int& bestAxisIndex) {
        float len2 = glm::length2(axis);
        // No collision
        if (len2 < 0.0001f) return true;

        // Calculating distance and overlap
        axis *= glm::inversesqrt(len2);

        const glm::vec3& eA = a.GetHalfExtents();
        const glm::vec3& eB = b.GetHalfExtents();

        float rA = eA.x * std::abs(glm::dot(a.GetAxis(0), axis)) +
            eA.y * std::abs(glm::dot(a.GetAxis(1), axis)) +
            eA.z * std::abs(glm::dot(a.GetAxis(2), axis));

        float rB = eB.x * std::abs(glm::dot(b.GetAxis(0), axis)) +
            eB.y * std::abs(glm::dot(b.GetAxis(1), axis)) +
            eB.z * std::abs(glm::dot(b.GetAxis(2), axis));

        float distance = std::abs(glm::dot(dist, axis));
        if (distance > rA + rB) return false;

        float overlap = (rA + rB) - distance;

        // Bias edge-edge cross axes (axisIndex >= 6) so face axes are strongly preferred
        const float edgeBias = (axisIndex >= 6) ? 0.005f : 0.0f;

        // If it is the axis with the minimun data update the contact
        if (overlap + edgeBias < minPenetration) {
            minPenetration = overlap;
            bestAxis = axis;
            bestAxisIndex = axisIndex;
        }
        return true;
    }

    // Helper to find the closes point between two lines
    void GetClosestPointLineSegments(const glm::vec3& p1, const glm::vec3& d1, float h1,
        const glm::vec3& p2, const glm::vec3& d2, float h2, glm::vec3& c1, glm::vec3& c2) {
        float b = glm::dot(d1, d2);
        float denominator = 1.0f - b * b;
        float t1 = 0.0f, t2 = 0.0f;
        glm::vec3 r = p1 - p2;
        float d = glm::dot(d1, r);
        float e = glm::dot(d2, r);

        if (denominator > 0.0001f) {
            t1 = (b * e - d) / denominator;
            t2 = (e - b * d) / denominator;
        }
        else {
            t1 = 0.0f;
            t2 = e;
        }

        t1 = glm::clamp(t1, -h1, h1);
        t2 = glm::dot(d2, p1 + d1 * t1 - p2);
        t2 = glm::clamp(t2, -h2, h2);
        t1 = glm::dot(d1, p2 + d2 * t2 - p1);
        t1 = glm::clamp(t1, -h1, h1);

        c1 = p1 + d1 * t1;
        c2 = p2 + d2 * t2;
    }

    // Helper to get the face vertices
    FixedPoly GetBoxFaceVertices(const BoxRb& box, int faceAxisIndex, float sign) {
        FixedPoly poly;
        poly.count = 4;
        const glm::vec3& h = box.GetHalfExtents();

        int u = (faceAxisIndex + 1) % 3;
        int v = (faceAxisIndex + 2) % 3;

        glm::vec3 boxPos = glm::vec3(box.mTransform->mPosition);
        glm::vec3 center = boxPos + box.GetAxis(faceAxisIndex) * (h[faceAxisIndex] * sign);
        glm::vec3 uVec = box.GetAxis(u) * h[u];
        glm::vec3 vVec = box.GetAxis(v) * h[v];

        poly.data[0] = center - uVec - vVec;
        poly.data[1] = center + uVec - vVec;
        poly.data[2] = center + uVec + vVec;
        poly.data[3] = center - uVec + vVec;

        return poly;
    }

    // Sutherland Hodgman clipping: iterate between vertices and check if they are inside a plane
    static inline FixedPoly ClipPolygonAgainstPlane(const FixedPoly& poly, const glm::vec3& planeNormal, float planeOffset) {
        FixedPoly clippedPoly;
        if (poly.count == 0) return clippedPoly;

        glm::vec3 v1 = poly.data[poly.count - 1];
        float d1 = glm::dot(planeNormal, v1) - planeOffset;

        for (int i = 0; i < poly.count; ++i) {
            const glm::vec3& v2 = poly.data[i];
            float d2 = glm::dot(planeNormal, v2) - planeOffset;

            if (d1 <= 0.0f && d2 <= 0.0f) {
                clippedPoly.push_back(v2);
            }
            else if (d1 <= 0.0f && d2 > 0.0f) {
                float t = d1 / (d1 - d2);
                clippedPoly.push_back(v1 + t * (v2 - v1));
            }
            else if (d1 > 0.0f && d2 <= 0.0f) {
                float t = d1 / (d1 - d2);
                clippedPoly.push_back(v1 + t * (v2 - v1));
                clippedPoly.push_back(v2);
            }
            v1 = v2;
            d1 = d2;
        }
        return clippedPoly;
    }

    // Box collisions
    bool BoxToBox(BoxRb* a, BoxRb* b, Contact& contact) {
        float minPenetration = FLT_MAX;
        glm::vec3 bestAxis(0.0f);
        int bestAxisIndex = -1;

        glm::vec3 posA = glm::vec3(a->mTransform->mPosition);
        glm::vec3 posB = glm::vec3(b->mTransform->mPosition);
        glm::vec3 dist = posB - posA;

        // SAT for each axis
        int axisCounter = 0;
        for (int i = 0; i < 3; ++i) {
            if (!TestAxis(*a, *b, a->GetAxis(i), dist, minPenetration, bestAxis, axisCounter, bestAxisIndex)) return false;
            axisCounter++;
        }
        for (int i = 0; i < 3; ++i) {
            if (!TestAxis(*a, *b, b->GetAxis(i), dist, minPenetration, bestAxis, axisCounter, bestAxisIndex)) return false;
            axisCounter++;


            if (bestAxisIndex >= 6) {

                glm::vec3 upB = b->GetAxis(1);
                if (std::abs(glm::dot(bestAxis, upB)) < 0.3f && dist.y > 0.0f) {
                    bestAxis = upB;
                }
            }
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    glm::vec3 axis = glm::cross(a->GetAxis(i), b->GetAxis(j));
                    if (!TestAxis(*a, *b, axis, dist, minPenetration, bestAxis, axisCounter, bestAxisIndex)) return false;
                    axisCounter++;
                }
            }

            // Normal much point from a to b
            if (glm::dot(bestAxis, dist) < 0.0f) bestAxis = -bestAxis;

            contact.bodyA = a;
            contact.bodyB = b;
            contact.normal = bestAxis;

            // it is a collision between edges
            if (bestAxisIndex >= 6) {
                int edgeIndexA = (bestAxisIndex - 6) / 3;
                int edgeIndexB = (bestAxisIndex - 6) % 3;

                glm::vec3 axisA = a->GetAxis(edgeIndexA);
                glm::vec3 axisB = b->GetAxis(edgeIndexB);

                glm::vec3 localNormalA = glm::transpose(a->GetRotationMatrix()) * contact.normal;
                glm::vec3 localNormalB = glm::transpose(b->GetRotationMatrix()) * (-contact.normal);

                glm::vec3 signA(localNormalA.x >= 0.0f ? 1.0f : -1.0f, localNormalA.y >= 0.0f ? 1.0f : -1.0f, localNormalA.z >= 0.0f ? 1.0f : -1.0f);
                glm::vec3 signB(localNormalB.x >= 0.0f ? 1.0f : -1.0f, localNormalB.y >= 0.0f ? 1.0f : -1.0f, localNormalB.z >= 0.0f ? 1.0f : -1.0f);

                const glm::vec3& halfA = a->GetHalfExtents();
                const glm::vec3& halfB = b->GetHalfExtents();

                glm::vec3 edgeCenterA = posA;
                for (int i = 0; i < 3; ++i) if (i != edgeIndexA) edgeCenterA += a->GetAxis(i) * (halfA[i] * signA[i]);

                glm::vec3 edgeCenterB = posB;
                for (int i = 0; i < 3; ++i) if (i != edgeIndexB) edgeCenterB += b->GetAxis(i) * (halfB[i] * signB[i]);

                glm::vec3 ptA, ptB;
                GetClosestPointLineSegments(edgeCenterA, axisA, halfA[edgeIndexA], edgeCenterB, axisB, halfB[edgeIndexB], ptA, ptB);

                contact.contactCount = 1;
                contact.points[0].position = (ptA + ptB) * 0.5f;
                contact.points[0].penetration = minPenetration;
                contact.points[0].normalImpulseSum = 0.0f;
                contact.points[0].tangentImpulseSum1 = 0.0f;
                contact.points[0].tangentImpulseSum2 = 0.0f;
                return true;
            }

            // Sutherland Hodgman 
            BoxRb* refBox = a;
            BoxRb* incBox = b;
            glm::vec3 refNormal = contact.normal;

            float maxDotA = -1.0f, maxDotB = -1.0f;
            int refAxisA = 0, refAxisB = 0;

            // The incident and reference faces
            for (int i = 0; i < 3; ++i) {
                float dA = std::abs(glm::dot(a->GetAxis(i), contact.normal));
                if (dA > maxDotA) { maxDotA = dA; refAxisA = i; }
                float dB = std::abs(glm::dot(b->GetAxis(i), contact.normal));
                if (dB > maxDotB) { maxDotB = dB; refAxisB = i; }
            }

            int refAxis = refAxisA;
            if (maxDotB > maxDotA) {
                refBox = b;
                incBox = a;
                refNormal = -contact.normal;
                refAxis = refAxisB;
            }

            int incAxis = 0;
            float maxAbsDot = -1.0f;
            for (int i = 0; i < 3; ++i) {
                float d = std::abs(glm::dot(incBox->GetAxis(i), refNormal));
                if (d > maxAbsDot) { maxAbsDot = d; incAxis = i; }
            }

            // Making sure we are choosing the right faces with signs
            float incSign = (glm::dot(incBox->GetAxis(incAxis), refNormal) < 0.0f) ? 1.0f : -1.0f;
            FixedPoly polygon = GetBoxFaceVertices(*incBox, incAxis, incSign);

            int uAxis = (refAxis + 1) % 3;
            int vAxis = (refAxis + 2) % 3;

            float refSign = (glm::dot(refBox->GetAxis(refAxis), refNormal) > 0.0f) ? 1.0f : -1.0f;
            glm::vec3 refFaceNormal = refBox->GetAxis(refAxis) * refSign;
            glm::vec3 posRef = glm::vec3(refBox->mTransform->mPosition);
            glm::vec3 refCenter = posRef + refFaceNormal * refBox->GetHalfExtents()[refAxis];

            glm::vec3 sideNormals[4] = { refBox->GetAxis(uAxis), -refBox->GetAxis(uAxis), refBox->GetAxis(vAxis), -refBox->GetAxis(vAxis) };
            float sideOffsets[4] = {
                glm::dot(sideNormals[0], refCenter + refBox->GetAxis(uAxis) * refBox->GetHalfExtents()[uAxis]),
                glm::dot(sideNormals[1], refCenter - refBox->GetAxis(uAxis) * refBox->GetHalfExtents()[uAxis]),
                glm::dot(sideNormals[2], refCenter + refBox->GetAxis(vAxis) * refBox->GetHalfExtents()[vAxis]),
                glm::dot(sideNormals[3], refCenter - refBox->GetAxis(vAxis) * refBox->GetHalfExtents()[vAxis])
            };

            // Clipping
            for (int i = 0; i < 4; ++i) {
                polygon = ClipPolygonAgainstPlane(polygon, sideNormals[i], sideOffsets[i]);
            }

            float refOffset = glm::dot(refFaceNormal, refCenter);
            int contactCount = 0;
            const float maxSeparation = 0.01f;

            // Extracting the contacts depending on the number of points
            for (int i = 0; i < polygon.count; ++i) {
                float separation = glm::dot(refFaceNormal, polygon.data[i]) - refOffset;
                if (separation <= maxSeparation) {
                    contact.points[contactCount].position = polygon.data[i];
                    contact.points[contactCount].penetration = glm::max(0.0f, -separation);
                    contact.points[contactCount].normalImpulseSum = 0.0f;
                    contact.points[contactCount].tangentImpulseSum1 = 0.0f;
                    contact.points[contactCount].tangentImpulseSum2 = 0.0f;

                    if (++contactCount >= 4) break;
                }
            }

            contact.contactCount = contactCount;
            return contactCount > 0;
        }
    }

    // Sphere and box collision
    bool SphereToBox(SphereRb* a, BoxRb* b, Contact& contact) {
        glm::vec3 posA = glm::vec3(a->mTransform->mPosition);
        glm::vec3 posB = glm::vec3(b->mTransform->mPosition);
        glm::vec3 relCenter = posA - posB;

        glm::vec3 localPos(
            glm::dot(relCenter, b->GetAxis(0)),
            glm::dot(relCenter, b->GetAxis(1)),
            glm::dot(relCenter, b->GetAxis(2))
        );

        const glm::vec3& extents = b->GetHalfExtents();

        glm::vec3 closestLocal(
            glm::clamp(localPos.x, -extents.x, extents.x),
            glm::clamp(localPos.y, -extents.y, extents.y),
            glm::clamp(localPos.z, -extents.z, extents.z)
        );

        glm::vec3 localV = closestLocal - localPos;
        float distSq = glm::length2(localV);

        // Inside box
        if (distSq < 0.000001f) {
            float dx = extents.x - std::abs(localPos.x);
            float dy = extents.y - std::abs(localPos.y);
            float dz = extents.z - std::abs(localPos.z);

            float minPen = dx;
            glm::vec3 worldNorm = (localPos.x >= 0.0f ? b->GetAxis(0) : -b->GetAxis(0));
            if (dy < minPen) { minPen = dy; worldNorm = (localPos.y >= 0.0f ? b->GetAxis(1) : -b->GetAxis(1)); }
            if (dz < minPen) { minPen = dz; worldNorm = (localPos.z >= 0.0f ? b->GetAxis(2) : -b->GetAxis(2)); }

            contact.bodyA = a;
            contact.bodyB = b;
            contact.contactCount = 1;
            contact.normal = worldNorm;
            contact.points[0].penetration = a->radius + minPen;
            contact.points[0].position = posA;
            contact.points[0].normalImpulseSum = 0.0f;
            contact.points[0].tangentImpulseSum1 = 0.0f;
            contact.points[0].tangentImpulseSum2 = 0.0f;
            return true;
        }

        if (distSq >= a->radius * a->radius) {
            return false;
        }

        float distance = std::sqrt(distSq);
        glm::vec3 localNormal = localV / distance;

        glm::vec3 worldNormal = localNormal.x * b->GetAxis(0) +
            localNormal.y * b->GetAxis(1) +
            localNormal.z * b->GetAxis(2);

        glm::vec3 closestPtWorld = posB +
            closestLocal.x * b->GetAxis(0) +
            closestLocal.y * b->GetAxis(1) +
            closestLocal.z * b->GetAxis(2);

        contact.bodyA = a;
        contact.bodyB = b;
        contact.contactCount = 1;
        contact.normal = worldNormal;
        contact.points[0].penetration = a->radius - distance;
        contact.points[0].position = closestPtWorld;
        contact.points[0].normalImpulseSum = 0.0f;
        contact.points[0].tangentImpulseSum1 = 0.0f;
        contact.points[0].tangentImpulseSum2 = 0.0f;

        return true;
    }
}