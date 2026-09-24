#pragma once

#include "../Dynamics/Rigidbody.h"
#include <vector>
#include <memory>
namespace clc {
    class CollisionPair {
    public:
        Rigidbody* bodyA = nullptr;
        Rigidbody* bodyB = nullptr;
    };

    class API Broadphase {
    public:
        Broadphase() = default;
        ~Broadphase() = default;

        void BuildPairs(const std::vector<Rigidbody*>& bodies, std::vector<CollisionPair>& outPairs);

    private:
        inline bool TestAABB(const Rigidbody* a, const Rigidbody* b) const {
            return (a->aabb.min.x <= b->aabb.max.x && a->aabb.max.x >= b->aabb.min.x) &&
                (a->aabb.min.y <= b->aabb.max.y && a->aabb.max.y >= b->aabb.min.y) &&
                (a->aabb.min.z <= b->aabb.max.z && a->aabb.max.z >= b->aabb.min.z);
        }
    };
}