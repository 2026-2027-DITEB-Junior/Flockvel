#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <memory>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include "../../Engine/IComp.h"
#include "../../Event System/EventSys.h"

namespace clc {
    enum class BodyType : unsigned char { Dynamic, Static, Kinematic };
    enum class ShapeType : unsigned char { Box, Sphere };
    enum Flags { None, Wall, Trigger };

    class AABB {
    public:
        glm::vec3 min{ 0.0f };
        glm::vec3 max{ 0.0f };
    };

    class Rigidbody : public IComp, public thrower {
    public:
        Rigidbody(BodyType bodyType = BodyType::Dynamic);
        virtual ~Rigidbody() = default;
        void AddToSystem() override;
        void RemoveFromSystem() override;
        void OnCreate() override;
        void SetMass(float mass_) {
            mass = mass_;
            invMass = 1.0f / mass;
        }

        float GetMass() {
            return mass;
        }

        inline void WakeUp() {
            if (type == BodyType::Dynamic && !isAwake) {
                isAwake = true;
                sleepTimer = 0.0f;
            }
        }

        inline void AddForce(const glm::vec3& force) {
            if (type == BodyType::Dynamic) {
                forceAccumulator += force;
                WakeUp();
            }
        }

        inline void AddTorque(const glm::vec3& torque) {
            if (type == BodyType::Dynamic && shape != ShapeType::Sphere) {
                torqueAccumulator += torque;
                WakeUp();
            }
        }

        inline void AddForceAtPoint(const glm::vec3& force, const glm::vec3& point) {
            if (type != BodyType::Dynamic) return;
            forceAccumulator += force;
            if (shape != ShapeType::Sphere) {
                torqueAccumulator += glm::cross(point - position, force);
            }
            WakeUp();
        }

        inline void ApplyLinearImpulse(const glm::vec3& impulse) {
            if (type == BodyType::Dynamic) {
                velocity += impulse * invMass;
                WakeUp();
            }
        }

        inline void ApplyAngularImpulse(const glm::vec3& impulse) {
            if (type == BodyType::Dynamic && shape != ShapeType::Sphere) {
                angularVelocity += invInertiaTensorWorld * impulse;
                WakeUp();
            }
        }

        inline void ApplyImpulseAtPoint(const glm::vec3& impulse, const glm::vec3& point) {
            if (type != BodyType::Dynamic) return;
            velocity += impulse * invMass;
            if (shape != ShapeType::Sphere) {
                angularVelocity += invInertiaTensorWorld * glm::cross(point - position, impulse);
            }
            WakeUp();
        }

        // Clears the force and torque (used each frame)
        inline void ClearAccumulators() {
            forceAccumulator = glm::vec3(0.0f);
            torqueAccumulator = glm::vec3(0.0f);
        }

        virtual void UpdateInertiaTensorWorld();
        void Update(float dt);
        virtual void CalculateInertia() = 0;
        virtual void UpdateAABB() = 0;

        glm::vec3 position{ 0.0f };
        glm::quat orientation{ 0.0f, 0.0f, 0.0f, 0.0f };
        glm::vec3 velocity{ 0.0f };
        glm::vec3 angularVelocity{ 0.0f };
        glm::vec3 forceAccumulator{ 0.0f };
        glm::vec3 torqueAccumulator{ 0.0f };

        glm::mat3 rotationMatrix{ 1.0f };
        glm::mat3 invInertiaTensorWorld{ 1.0f };
        glm::mat3 invInertiaTensorLocal{ 1.0f };

        Transform* mTransform = nullptr;
        AABB aabb;

        float restitution = 0.5f;
        float linearDamping = 0.98f;
        float angularDamping = 0.95f;
        float sleepTimer = 0.0f;

        BodyType type = BodyType::Dynamic;
        ShapeType shape;
        Flags eventType;
        bool useGravity = true;
        bool isAwake = true;
        bool isColliding = false;
        float invMass = 1.0f;
    private:        
        float mass = 1.0f;

    };
}