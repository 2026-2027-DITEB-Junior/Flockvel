#pragma once

#include "Rigidbody.h"

namespace clc {
    class API BoxRb : public Rigidbody {
    public:
        BoxRb(BodyType bodyType = BodyType::Static, 
            const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& boxSize = glm::vec3(1.0f), float bodyMass = 1.0f);
        ~BoxRb() override = default;
        void CalculateInertia() override;
        void UpdateAABB() override;

        void OnCreate() override;

        glm::vec3 GetHalfExtents() const { return halfExtents; }
        glm::mat3 GetRotationMatrix() const { return rotationMatrix; }
        glm::vec3 GetAxis(int index) const { return rotationMatrix[index]; }

        glm::vec3 size{ 1.0f, 1.0f, 1.0f };
        glm::vec3 halfExtents{ 0.5f, 0.5f, 0.5f };
    };
}