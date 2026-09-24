#include "pch.h"
#include "SphereRb.h"
#include "../PhysicsSystem.h"
using namespace clc;
SphereRb::SphereRb(BodyType bodyType, const glm::vec3& pos, float sphereRadius, float bodyMass)
    : Rigidbody(bodyType), radius(sphereRadius) {
    position = pos;
    SetMass(bodyMass);
    shape = ShapeType::Sphere;
    CalculateInertia();
    UpdateInertiaTensorWorld();
}

// Inertia of a sphere
void SphereRb::CalculateInertia() {
    if (type != BodyType::Dynamic) {
        invMass = 0.0f;
        invInertiaTensorLocal = glm::mat3(0.0f);
        return;
    }

    float mass = GetMass();
    // Same as in PHY 200 2 / 5 * m * d^2
    const float i = (2.0f / 5.0f) * mass * (radius * radius);
    const float invI = 1.0f / i;

    invInertiaTensorLocal = glm::mat3(0.0f);
    invInertiaTensorLocal[0][0] = invI;
    invInertiaTensorLocal[1][1] = invI;
    invInertiaTensorLocal[2][2] = invI;
}

// The inertia does not change
void SphereRb::UpdateInertiaTensorWorld() {
    invInertiaTensorWorld = invInertiaTensorLocal;
}

// Update for the AABB
void SphereRb::UpdateAABB() {
    aabb.min = glm::vec3(mTransform->mPosition - glm::vec3(radius,radius, radius));
    aabb.max = glm::vec3(mTransform->mPosition + glm::vec3(radius, radius, radius));
}