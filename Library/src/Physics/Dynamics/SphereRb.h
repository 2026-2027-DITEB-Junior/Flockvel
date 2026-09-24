#pragma once

#include "Rigidbody.h"

namespace clc {
    class API SphereRb : public Rigidbody {
    public:
        SphereRb(BodyType bodyType = BodyType::Static, const glm::vec3& pos = glm::vec3(0.0f), float sphereRadius = 0.5f, float bodyMass = 1.0f);

        ~SphereRb() override = default;

        void CalculateInertia() override;
        void UpdateAABB() override;
        void UpdateInertiaTensorWorld() override;

        float radius{ 0.5f };
    };
}