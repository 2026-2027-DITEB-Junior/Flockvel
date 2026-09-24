#include "pch.h"
#include "RTTI.h"

namespace clc
{
    const std::string Rtti::GetType() const {
        assert(_typename.size()); // "Rtti::GetType() non const hasn't been called first");
        return _typename;
    }
    const std::string Rtti::GetType() {

        // lazy init. _typename is empty
        if (_typename.empty())
        {
            // get the type name using c++ rtti system
            _typename = typeid(*this).name();

            // process to remove 'class' or 'struct'
            std::size_t pos = _typename.find_last_of(' ');
            _typename = _typename.substr(pos + 1);

            // remove namespace as well, if any.
            pos = _typename.find_last_of(':');
            if (pos != std::string::npos)
                _typename = _typename.substr(pos + 1);
        }

        // return stored typename
        return _typename;
    }
}