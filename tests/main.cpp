#include <Mono/Mono.h>
#include <Mono/Compiler.h>
#include <Mono/Assembly.h>
#include <Mono/Object.h>

#include <iostream>

static int Log()
{
    int msg = 10;
    std::cout << msg << "\n";
    return msg;
}

int main()
{
    Mono::init("/usr/lib", "/etc", "Application");

    Mono::Compiler compiler("/usr/bin/mcs");
    compiler.buildLibrary("Tests.dll", "Tests.cs");

    Mono::Assembly assembly(Mono::getCurrentDomain(), "Tests.dll");

    Mono::addInternalCall<int()>("Tests.TestClass::Log()", MONO_BIND_FN(Log));
    //mono_add_internal_call("Tests.TestClass::Log(int)", (const void*)Log);

    Mono::Type type(assembly.getImage(), "Tests", "TestClass");
    Mono::Object object(Mono::getCurrentDomain(), type, "::.ctor(int)", 5);
    auto powMethod = object.getMethod<void(std::string, std::string)>("::add(string,string)");
    powMethod.invokeInstance<void(std::string, std::string)>(object, "Hello, ", "world!");



    //object["price"] = 10;
    std::cout << (int)object["price"] << "\n";

    return 0;
}