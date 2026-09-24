#include "pch.h"
#include "Broadphase.h"

using namespace clc;
// Used to have less checks on a narrowphase (obj vs obj), uses only AABB 
void Broadphase::BuildPairs(const std::vector<Rigidbody*>& bodies, std::vector<CollisionPair>& outPairs) {
    outPairs.clear();
    const size_t numBodies = bodies.size();

    for (size_t i = 0; i < numBodies; ++i) {
        Rigidbody* bodyA = bodies[i];

        for (size_t j = i + 1; j < numBodies; ++j) {
            Rigidbody* bodyB = bodies[j];

            // Skipping pairs of static objects
            if (bodyA->type != BodyType::Dynamic && bodyB->type != BodyType::Dynamic) {
                continue;
            }

            // Skipping pairs of objects that did not move for several frames
            if (!bodyA->isAwake && !bodyB->isAwake) {
                continue;
            }

            // Test min and max of the AABB
            if (TestAABB(bodyA, bodyB)) {
                outPairs.push_back({ bodyA, bodyB });
            }
        }
    }
}