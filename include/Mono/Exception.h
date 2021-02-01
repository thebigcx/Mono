#pragma once

#include <string>
#include <stdexcept>

#include <mono/jit/jit.h>

namespace Mono
{

class Exception : public std::runtime_error
{
public:
    Exception(const std::string& what)
        : std::runtime_error(what) {}
};

}