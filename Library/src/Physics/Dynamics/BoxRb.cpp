#include "pch.h"
#include "BoxRb.h"
#include "../PhysicsSystem.h"

using namespace clc;
BoxRb::BoxRb(BodyType bodyType, const glm::vec3& pos, const glm::vec3& boxSize, float bodyMass)
    : Rigidbody(bodyType), size(boxSize), halfExtents(boxSize * 0.5f) {
    position = pos;
    SetMass(bodyMass);
    shape = ShapeType::Box;
    CalculateInertia();
    UpdateInertiaTensorWorld();
    
}

void BoxRb::OnCreate() { 
    Rigidbody::OnCreate(); 
    size = glm::vec3(mTransform->mScale / 2.0f);
    halfExtents = size;
    UpdateAABB();
}

// Inertia of a box
void BoxRb::CalculateInertia() {
    if (type != BodyType::Dynamic) {
        invMass = 0.0f;
        invInertiaTensorLocal = glm::mat3(0.0f);
        return;
    }

    float mass = GetMass();

    // Different each axis but it is the same formula as in PHY200 1 /12 * m * d^2
    const float iX = (1.0f / 12.0f) * mass * (size.y * size.y + size.z * size.z);
    const float iY = (1.0f / 12.0f) * mass * (size.x * size.x + size.z * size.z);
    const float iZ = (1.0f / 12.0f) * mass * (size.x * size.x + size.y * size.y);

    invInertiaTensorLocal = glm::mat3(0.0f);
    invInertiaTensorLocal[0][0] = 1.0f / iX;
    invInertiaTensorLocal[1][1] = 1.0f / iY;
    invInertiaTensorLocal[2][2] = 1.0f / iZ;
}

// Updating the AABB for broadphase
void BoxRb::UpdateAABB() {
    const glm::vec3 absX = glm::abs(rotationMatrix[0]) * halfExtents.x;
    const glm::vec3 absY = glm::abs(rotationMatrix[1]) * halfExtents.y;
    const glm::vec3 absZ = glm::abs(rotationMatrix[2]) * halfExtents.z;
    const glm::vec3 worldHalfExtents = absX + absY + absZ;
    glm::vec3 pos = glm::vec3(mTransform->mPosition);
    aabb.min = pos - worldHalfExtents;
    aabb.max = pos + worldHalfExtents;
}