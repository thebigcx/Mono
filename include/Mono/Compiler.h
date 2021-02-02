#pragma once

#include <string>

namespace Mono
{

class Compiler
{
public:
    Compiler(const std::string& compilerPath)
        : m_compilerPath(compilerPath) {}

    void buildLibrary(const std::string& target, const std::string& script)
    {
        std::string command = m_compilerPath + " -out:" + target + " -target:library " + script;
        std::system(command.c_str());
    }

private:
    std::string m_compilerPath;
};

}