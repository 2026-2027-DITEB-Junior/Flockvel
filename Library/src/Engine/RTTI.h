#pragma once
#include "../API.h"
#include <string>
#include <typeinfo>
#include <assert.h>

namespace clc
{
    // Rtti is a class that provides Run-Time Type information for the 
    // classes of aex Engine.
    class API Rtti {
    private:

        // stores the type name to avoid processing it everytime GetType is called. 
        // It is initialized the first time GetType is called. 
        std::string _typename;
    public:
        virtual ~Rtti() = default;

        // Returns the exact runtime type of the object
        const std::string GetType() const;
        const std::string GetType();

        // Templated function to check if this object is exactly of a specific type T
        template <typename T>
        bool IsExactly() const {
            return typeid(*this) == typeid(T);
        }

        // Check if this object is a descendant of a specific base class T
        template <typename T>
        bool IsDerived() const {
            return dynamic_cast<const T*>(this) != nullptr;
        }

        // Static function to be able to get the string type from 
        // any class by type instead of by instance. 
        // Just as in the other "GetType()", it uses typeid().name()
        // and removes the 'class ' and namespaces/
        // example: Rtti::GetType<aex::game::Player>(); // returns "Player"
        template <typename T> static std::string GetType() {
            // get the type name using c++ rtti system
            std::string strType = typeid(T).name();

            // process to remove 'class' or 'struct'
            std::size_t pos = strType.find_last_of(' ');
            strType = strType.substr(pos + 1);

            // remove namespace as well, if any.
            pos = strType.find_last_of(':');
            if (pos != std::string::npos)
                strType = strType.substr(pos + 1);

            // return 
            return strType;
        }
    };
}