#pragma once

#include <string>

namespace Mono
{

class Compiler
{
public:
    Compiler(const std::string& compilerPath);

    void buildLibrary(const std::string& target, const std::string& script);

private:
    std::string m_compilerPath;
};

}