using System;
using System.Runtime.CompilerServices;

namespace Engine
{

public class GameComponent
{

};

public class Transform : GameComponent
{
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public static extern void SetTransformX_Internal(float x);

    private float m_x, m_y, m_z;
    
    public float x
    {
        get { return m_x; }
        set { SetTransformX_Internal(value); m_x = value; }
    }

    public float y
    {
        get { return m_y; }
        set {  }
    }

    public float z
    {
        get { return m_z; }
        set {  }
    }
};

public class GameObject
{
    public T GetComponent<T>()
    {
        
    }

    public static unsafe Transform GetTransform()
    {
        return GetTransform_Internal();
    }

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public static extern ref Transform GetTransform_Internal();
};

}

namespace Tests
{

public class TestClass : Engine.GameObject
{
    public TestClass()
    {
        
    }

    public void onUpdate()
    {
        var transform = GetTransform();
        transform.x = 10;
    }
};

}