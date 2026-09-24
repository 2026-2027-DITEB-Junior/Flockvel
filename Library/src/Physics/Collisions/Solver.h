#pragma once

#include "Contact.h"
#include <vector>
#ifdef IN_LIBRARY
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif
namespace clc {

        void API SolveContact(std::vector<Contact>& contacts, float dt);
        void API SolvePositionConstraints(std::vector<Contact>& contacts);
    
}