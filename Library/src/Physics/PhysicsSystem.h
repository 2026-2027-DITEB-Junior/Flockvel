#pragma once
#include "Dynamics/Rigidbody.h"
#include "Collisions/Contact.h"
#include "Collisions/Broadphase.h"
#include <vector>
#include <memory>
#include "../Engine/Singleton.h"

namespace clc {
    class API PhysicsSystem {
        CLC_SINGLETON(PhysicsSystem);
    public:

        void AddBody(Rigidbody* body);
        void RemoveBody(Rigidbody* body);
        void Shutdown();
        void Update(double dt);

        glm::vec3 gravity{ 0.0f, -73.0f, 0.0f };
        std::vector<Rigidbody*> bodies;

    private:
        void CheckCollisions();

        float fixedDt = 1.0f / 60.0f;
        float curTime = 0.0f;

        Broadphase broadphase;
        std::vector<CollisionPair> potentialPairs;
        std::vector<Contact> contacts;
        std::vector<Contact> oldContacts;
    };
}

#define clcPhysics (clc::PhysicsSystem::GetInstance())