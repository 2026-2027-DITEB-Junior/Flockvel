#include "pch.h"
#include "Solver.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <algorithm>

namespace clc {
    // Solves velocity using sequential impulses depending on masses 
    // (makes them more accurate with Gauss-Seidel)
    void SolveContact(std::vector<Contact>& contacts, float dt) {
        // Setting the iterations,
        // TODO: MUST BE CHANGE, too big of a number,
        // try reducing the iteration number until it is
        // no longer a performance issue and it still solves well
        const int iterations = 15;
        const float frictionCoeff = 0.2f;
        const float beta = 0.2f;
        const float slop = 0.001f;

        // Loop through contacts
        for (auto& contact : contacts) {
            Rigidbody* bodyA = contact.bodyA;
            Rigidbody* bodyB = contact.bodyB;
            const glm::vec3& normal = contact.normal;

            for (int i = 0; i < contact.contactCount; ++i) {
                auto& pt = contact.points[i];

                // Needed for torque (it goes from center of mass to the contact points)
                pt.rA = pt.position - glm::vec3(bodyA->mTransform->mPosition);
                pt.rB = pt.position - glm::vec3(bodyB->mTransform->mPosition);

                // Using this to do Gram-Schmidt last steps
                if (normal.x * normal.x > 1.0f / 9.0f) {
                    pt.T1 = glm::normalize(glm::vec3(normal.y, -normal.x, 0.0f));
                }
                else {
                    pt.T1 = glm::normalize(glm::vec3(0.0f, normal.z, -normal.y));
                }
                pt.T2 = glm::cross(normal, pt.T1);

                // Calculating the efective masses
                glm::vec3 rACrossN = glm::cross(pt.rA, normal);
                glm::vec3 rBCrossN = glm::cross(pt.rB, normal);
                float angularAN = glm::dot(rACrossN, bodyA->invInertiaTensorWorld * rACrossN);
                float angularBN = glm::dot(rBCrossN, bodyB->invInertiaTensorWorld * rBCrossN);
                pt.invMassSumN = bodyA->invMass + bodyB->invMass + angularAN + angularBN;

                glm::vec3 rACrossT1 = glm::cross(pt.rA, pt.T1);
                glm::vec3 rBCrossT1 = glm::cross(pt.rB, pt.T1);
                pt.invMassSumT1 = bodyA->invMass + bodyB->invMass +
                    glm::dot(rACrossT1, bodyA->invInertiaTensorWorld * rACrossT1) +
                    glm::dot(rBCrossT1, bodyB->invInertiaTensorWorld * rBCrossT1);

                glm::vec3 rACrossT2 = glm::cross(pt.rA, pt.T2);
                glm::vec3 rBCrossT2 = glm::cross(pt.rB, pt.T2);
                pt.invMassSumT2 = bodyA->invMass + bodyB->invMass +
                    glm::dot(rACrossT2, bodyA->invInertiaTensorWorld * rACrossT2) +
                    glm::dot(rBCrossT2, bodyB->invInertiaTensorWorld * rBCrossT2);

                // Adding the impulses from last frame
                glm::vec3 impulseN = normal * pt.normalImpulseSum;
                bodyA->ApplyImpulseAtPoint(-impulseN, pt.position);
                bodyB->ApplyImpulseAtPoint(impulseN, pt.position);
            }
        }

        // Current frame solver (Here Gauss-Seidel is applied)
        for (int iter = 0; iter < iterations; ++iter) {
            for (auto& contact : contacts) {
                Rigidbody* bodyA = contact.bodyA;
                Rigidbody* bodyB = contact.bodyB;
                const glm::vec3& N = contact.normal;

                for (int i = 0; i < contact.contactCount; ++i) {
                    auto& pt = contact.points[i];

                    // Relative velocities
                    glm::vec3 vA = bodyA->velocity + glm::cross(bodyA->angularVelocity, pt.rA);
                    glm::vec3 vB = bodyB->velocity + glm::cross(bodyB->angularVelocity, pt.rB);
                    float vn = glm::dot(vB - vA, N);

                    // Adding impulses for normal
                    if (pt.invMassSumN > 0.0f) {
                        float e = (vn < -1.0f) ? std::min(bodyA->restitution, bodyB->restitution) : 0.0f;
                        float deltaImpulseN = (-(1.0f + e) * vn + pt.bias) / pt.invMassSumN;

                        float oldImpulseN = pt.normalImpulseSum;
                        pt.normalImpulseSum = glm::max(0.0f, oldImpulseN + deltaImpulseN);
                        deltaImpulseN = pt.normalImpulseSum - oldImpulseN;

                        glm::vec3 impulseN = N * deltaImpulseN;
                        bodyA->ApplyImpulseAtPoint(-impulseN, pt.position);
                        bodyB->ApplyImpulseAtPoint(impulseN, pt.position);
                    }

                    vA = bodyA->velocity + glm::cross(bodyA->angularVelocity, pt.rA);
                    vB = bodyB->velocity + glm::cross(bodyB->angularVelocity, pt.rB);
                    float vt1 = glm::dot(vB - vA, pt.T1);

                   

                    vA = bodyA->velocity + glm::cross(bodyA->angularVelocity, pt.rA);
                    vB = bodyB->velocity + glm::cross(bodyB->angularVelocity, pt.rB);
                    float vt2 = glm::dot(vB - vA, pt.T2);
                }
            }
        }
    }

    // Solves the penetration problems
    void SolvePositionConstraints(std::vector<Contact>& contacts) {
        const float slop = 0.003f;   
        const float percent = 0.3f;  

        for (auto& contact : contacts) {
            Rigidbody* bodyA = contact.bodyA;
            Rigidbody* bodyB = contact.bodyB;
            const glm::vec3& N = contact.normal;

            float totalInvMass = bodyA->invMass + bodyB->invMass;

            // This means that is static
            if (totalInvMass <= 0.0f) continue;

            float invContactCount = 1.0f / static_cast<float>(contact.contactCount);

            for (int i = 0; i < contact.contactCount; ++i) {
                float penetration = contact.points[i].penetration - slop;

                //Solve position if the penetration is positive
                if (penetration > 0.0f) {
                    glm::vec3 correction = N * (penetration * percent * invContactCount);

                    if (bodyA->type == BodyType::Dynamic) {
                        bodyA->mTransform->mPosition -= glm::vec4(correction, 0.0f) * (bodyA->invMass / totalInvMass);
                        bodyA->position = glm::vec3(bodyA->mTransform->mPosition);
                        bodyA->UpdateAABB();
                    }
                    if (bodyB->type == BodyType::Dynamic) {
                        bodyB->mTransform->mPosition += glm::vec4(correction, 0.0f) * (bodyB->invMass / totalInvMass);
                        bodyB->position = glm::vec3(bodyB->mTransform->mPosition);
                        bodyB->UpdateAABB();
                    }
                }
            }
        }
    }
}