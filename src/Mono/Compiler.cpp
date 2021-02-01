#include <Mono/Compiler.h>

namespace Mono
{

Compiler::Compiler(const std::string& compilerPath)
    : m_compilerPath(compilerPath)
{

}

void Compiler::buildLibrary(const std::string& target, const std::string& script)
{
    std::string command = m_compilerPath + " -out:" + target + " -target:library " + script;
    std::system(command.c_str());
}

}