#pragma once

#include <string>

namespace Mono
{

class Compiler
{
public:
    Compiler(const std::string& compilerPath)
        : m_compilerPath(compilerPath) {}

    template<typename... Args>
    void buildLibrary(const std::string& target, Args&&... args)
    {
        std::string libs = "";
        (expandLibraries(args, libs), ...);
        std::string command = m_compilerPath + " -unsafe -out:" + target + " -target:library " + libs;
        std::system(command.c_str());
    }

private:
    std::string m_compilerPath;

    void expandLibraries(const std::string& lib, std::string& dest)
    {
        dest += lib + " ";
    }
};

}