using System;
using System.Runtime.CompilerServices;

namespace Tests
{

public class TestClass
{
    public int price;

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public static extern void helloFromCpp();

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

    public static void add(string a, string b)
    {
        string result = a + b;
        //return result;
    }
};

}