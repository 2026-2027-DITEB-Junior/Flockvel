#include "pch.h"
#include "PhysicsSystem.h"
#include "Dynamics/BoxRb.h"
#include "Dynamics/SphereRb.h"
#include "Collisions/Collisions.h"
#include "Collisions/Solver.h"
#include <algorithm>

using namespace clc;
    void PhysicsSystem::AddBody(Rigidbody* body) {
        if (body && std::find(bodies.begin(), bodies.end(), body) == bodies.end()) bodies.push_back(body);
    }
    void PhysicsSystem::RemoveBody(Rigidbody* body) {
        bodies.erase(std::remove(bodies.begin(), bodies.end(), body), bodies.end());
        potentialPairs.clear();
        contacts.clear();
        oldContacts.clear();
        if (bodies.empty()) curTime = 0.0f;
    }
    void PhysicsSystem::Shutdown() {
        bodies.clear();
        potentialPairs.clear();
        contacts.clear();
        oldContacts.clear();
        curTime = 0.0f;
    }

    // Update with at fixed delta time 
    void PhysicsSystem::Update(double dt) {
        // Clamping to 0.1 so it is stable
        dt = std::min(dt, 0.1);
        curTime += dt;

        const glm::vec3 gravityStep = gravity * fixedDt;
        int steps = 0;
        const int maxSubSteps = 5;

        // Check for substepping and delta time (with a max of substeps)
        while (curTime >= fixedDt && steps < maxSubSteps) {

            // Adding gravity before anything to all dynamic bodies
            // (if added later you have problems)
            for (auto& body : bodies) {
                if (body->type == BodyType::Dynamic && body->isAwake) {
                    if (body->useGravity) {
                        body->velocity += gravityStep * body->GetMass();
                    }
                }
            }

            // Check an solve collisions (not postions)
            CheckCollisions();
            SolveContact(contacts, fixedDt);

            // Update all rigidbodies
            for (auto& body : bodies) {
                body->Update(fixedDt);
            }

            SolvePositionConstraints(contacts);

            curTime -= fixedDt;
            ++steps;
        }

        if (steps >= maxSubSteps) {
            curTime = 0.0f;
        }
    }


    // Checks collisions with broadphase and narrowphase
    void PhysicsSystem::CheckCollisions() {
        contacts.clear();

        // Reset collision flag
        for (auto& body : bodies) {
            body->isColliding = false;
        }

        // Broadphase with AABB as bounding volumes
        broadphase.BuildPairs(bodies, potentialPairs);

        if (contacts.capacity() < potentialPairs.size()) {
            contacts.reserve(potentialPairs.size());
        }

        // Narrowphase object vs object
        for (const auto& pair : potentialPairs) {
            Rigidbody* bodyA = pair.bodyA;
            Rigidbody* bodyB = pair.bodyB;

            Contact contact;
            bool collided = false;

            /// TODO: Optimize this (its the biggest bottleneck for now)
            if (bodyA->shape == ShapeType::Box && bodyB->shape == ShapeType::Box) {
                auto* boxA = static_cast<BoxRb*>(bodyA);
                auto* boxB = static_cast<BoxRb*>(bodyB);
                collided = BoxToBox(boxA, boxB, contact);
            }
            else if (bodyA->shape == ShapeType::Sphere && bodyB->shape == ShapeType::Sphere) {
                auto* sphereA = static_cast<SphereRb*>(bodyA);
                auto* sphereB = static_cast<SphereRb*>(bodyB);
                collided = SphereToSphere(sphereA, sphereB, contact);
            }
            else {
                SphereRb* sphere;
                BoxRb* box;
                if (bodyA->shape == ShapeType::Sphere) {
                    sphere = static_cast<SphereRb*>(bodyA);
                    box = static_cast<BoxRb*>(bodyB);
                }
                else {
                    sphere = static_cast<SphereRb*>(bodyB);
                    box = static_cast<BoxRb*>(bodyA);
                }
                collided = SphereToBox(sphere, box, contact);
            }

            if (collided) {
                bodyA->WakeUp();
                bodyB->WakeUp();
                //this is what u do to throw a event (THANKS ALEX)
                //for events with a flag and vec3

                FlagEvent* flagA = new FlagEvent("Collision", bodyB->eventType, contact.normal);
                FlagEvent* flagB = new FlagEvent("Collision", bodyA->eventType, -contact.normal);

                bodyA->throw_event(flagA);
                bodyB->throw_event(flagB);

                bodyA->isColliding = true;
                bodyB->isColliding = true;

                if (bodyA->eventType == Flags::Trigger) {
                    bodyA->throw_event("Trigger");
                }
                else if (bodyB->eventType == Flags::Trigger) {
                    bodyB->throw_event("Trigger");
                }
                else {
                    contacts.push_back(contact);
                }
            }
        }
    }
