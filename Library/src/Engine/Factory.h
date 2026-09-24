#pragma once
#include "../API.h"
#include <unordered_map>
#include "Serialization.h"
#include "Singleton.h"
#include "RTTI.h"

namespace clc
{
    // forward declare IBase
    class IBase;

    // Allocator base class - Pure interface
    class Allocator {
    public:
        virtual ~Allocator() = default;
        virtual IBase* Create() = 0;
    };

    // Templated allocator - Implementation of Allocator
    // Create only calls new T
    template <typename T> class TAllocator : public Allocator {
    public:
        virtual IBase* Create() {
            return new T;
        }
    };

    // Factory singleton
    class API Factory
    {
        CLC_SINGLETON(Factory);

    public:
        bool Initialize();

        // Main Factory function: Given a string representing a type
        // it allocates that type and returns it. 
        // Internally it tries to find the corresponding allocator 
        // in the map and calls the function `Create` on it. 
        IBase* Create(const std::string& type);

        // Register function is necessary in order to add an allocator
        // for a specific type. Internally, the function insures there 
        // are no duplicates in the map, and if not, adds the `allocator`
        // into mAllocators.
        void Register(const std::string& type, Allocator* allocator);

        // Template version of the register function. 
        // It uses Rtti to get the type as a string and adds
        // a new TAllocator directly. 
        // It calls the `Register` function above. 
        template <typename T> void Register();

        // Convenience function to create any class previously 
        // registered with the factory. This a little slower
        // than using `new T`, but it will centralize all 
        // allocations to one class (the allocator for that type). 
        template <typename T> T* Create();

        //mAllocators string getter: for ImGui component manager mainly
        std::vector<std::string> getAllocatedComponents();


        // Creates/deserializes data only. Caller runs OnCreate/AddToSystem/Start after the graph is complete.
        IBase* CreateFromJson(const json& j, const std::string& defaultType = std::string());

    private:
        std::unordered_map<std::string, Allocator*> mAllocators;
    };
}

#include "Factory.inl"

#define clcFactory (clc::Factory::GetInstance())