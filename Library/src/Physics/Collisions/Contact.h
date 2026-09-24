#pragma once

#include "../Dynamics/Rigidbody.h"
#include <glm/glm.hpp>

namespace clc {
    class API ContactPoint {
    public:
        glm::vec3 position;
        float penetration;

        float normalImpulseSum = 0.0f;
        float tangentImpulseSum1 = 0.0f;
        float tangentImpulseSum2 = 0.0f;

        glm::vec3 rA, rB;
        glm::vec3 T1, T2;
        float invMassSumN = 0.0f;
        float invMassSumT1 = 0.0f;
        float invMassSumT2 = 0.0f;
        float bias = 0.0f;
    };

    class API Contact {
    public:
        Rigidbody* bodyA = nullptr;
        Rigidbody* bodyB = nullptr;

        glm::vec3 normal{ 0.0f };
        ContactPoint points[4];
        int contactCount = 0;
    };
}