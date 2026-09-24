#include "pch.h"
#include "Rigidbody.h"
#include "../PhysicsSystem.h"
using namespace clc;
Rigidbody::Rigidbody(BodyType bodyType) : type(bodyType) {
    if (type != BodyType::Dynamic) {
        mass = 0.0f;
        invMass = 0.0f;
        invInertiaTensorLocal = glm::mat3(0.0f);
        invInertiaTensorWorld = glm::mat3(0.0f);
        rotationMatrix = glm::mat3(1.0f);
        isAwake = false;
    }
    
}

void Rigidbody::AddToSystem() {
    clcPhysics.AddBody(this);
}

void Rigidbody::OnCreate() {
    mTransform = &Owner()->mTransform;
    position = mTransform->mPosition;
    orientation = mTransform->mRotation;
    UpdateInertiaTensorWorld();
    UpdateAABB();
}

// Update the inertia matrix
void Rigidbody::UpdateInertiaTensorWorld() {
    rotationMatrix = glm::mat3_cast(orientation);

    if (type != BodyType::Dynamic) {
        invInertiaTensorWorld = glm::mat3(0.0f);
        return;
    }

    invInertiaTensorWorld = rotationMatrix * invInertiaTensorLocal * glm::transpose(rotationMatrix);
}

// Update for any Rigidbody
void Rigidbody::Update(float dt) {
    if (type != BodyType::Dynamic || !isAwake) return;
    // Moving the velocity by acceleration * dt; same for angular but with inertia * torque 
    velocity += (forceAccumulator * invMass) * dt;
    angularVelocity += (invInertiaTensorWorld * torqueAccumulator) * dt;
    ClearAccumulators();

    mTransform->mPosition += glm::vec4(velocity.x, velocity.y, velocity.z, 0.0f) * dt;


    // Damping
    const float dtScale = dt * 60.0f;
    const float lDamp = (dtScale == 1.0f) ? linearDamping : std::pow(linearDamping, dtScale);
    const float aDamp = (dtScale == 1.0f) ? angularDamping : std::pow(angularDamping, dtScale);
    velocity *= lDamp;
    angularVelocity *= aDamp;

    // Sleep rigidbodies that have not move for several seconds,
    // so we do not update objects that are not moving
    const float sleepThresholdSq = 0.005f;
    const float currentMotion = glm::length2(velocity) + glm::length2(angularVelocity);

    if (currentMotion < sleepThresholdSq) {
        sleepTimer += dt;
        if (sleepTimer >= 0.5f) {
            isAwake = false;
            velocity = glm::vec3(0.0f);
            angularVelocity = glm::vec3(0.0f);
            sleepTimer = 0.0f;
            return; 
        }
    }
    else {
        sleepTimer = 0.0f;
    }
    position = mTransform->mPosition;
    // Inertia and AABB for broadphase update
    UpdateInertiaTensorWorld();
    UpdateAABB();
}
void clc::Rigidbody::RemoveFromSystem() { clcPhysics.RemoveBody(this); }
