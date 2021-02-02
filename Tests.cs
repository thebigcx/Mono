using System;
using System.Runtime.CompilerServices;

namespace Tests
{

public class TestClass
{
    public int price;

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public static extern int Log();

    public TestClass()
    {
        //Console.WriteLine("Hello from C#!");
    }

    public TestClass(int id)
    {
        //Console.WriteLine(id);
    }

    public static void helloFromCSharp()
    {
        //Console.WriteLine("Hello from C#!");
    }

    public void add(string a, string b)
    {
        price = Log();
        //return result;
    }
};

}