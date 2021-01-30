#pragma once

#include <mono/jit/jit.h>

namespace Engine
{
namespace Mono
{

class Image
{
public:
    Image() = default;
    Image(MonoImage* image)
        : m_image(image) {}

    MonoImage* get() const { return m_image; }

private:
    MonoImage* m_image = nullptr;
};

}
}