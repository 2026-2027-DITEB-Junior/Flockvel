#pragma once
#include <string>
#include <typeinfo>
#include <assert.h>

namespace clc {
    namespace GL
    {

        // Rtti is a class that provides Run-Time Type information for the 
        // classes of aex Engine.
        class API Rtti {
        private:
        public:
            virtual ~Rtti() = default;

            // Returns the exact runtime type of the object
            std::string GetType() const;

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
        };
    }
}
