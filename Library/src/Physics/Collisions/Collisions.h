#pragma once

#include "Contact.h"
#include "../Dynamics/BoxRb.h"
#include "../Dynamics/SphereRb.h"

namespace clc {
    // Helper struct
    class API FixedPoly {
    public:
        std::array<glm::vec3, 8> data;
        int count = 0;

        void push_back(const glm::vec3& v) {
            if (count < 8) data[count++] = v;
        }
    };

    bool API TestAxis(const BoxRb& a, const BoxRb& b, glm::vec3 axis, glm::vec3& dist, float& minPenetration, glm::vec3& bestAxis, int axisIndex, int& bestAxisIndex);
    void API GetClosestPointLineSegments(const glm::vec3& p1, const glm::vec3& d1, float h1, const glm::vec3& p2, const glm::vec3& d2, float h2, glm::vec3& c1, glm::vec3& c2);
    FixedPoly API GetBoxFaceVertices(const BoxRb& box, int faceAxisIndex, float sign);
    bool API BoxToBox(BoxRb* a, BoxRb* b, Contact& contact);
    bool API SphereToSphere(SphereRb* a, SphereRb* b, Contact& contact);
    bool API SphereToBox(SphereRb* a, BoxRb* b, Contact& contact);


    
}