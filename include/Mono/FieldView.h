#pragma once

#include <mono/jit/jit.h>

namespace Mono
{

class FieldView
{
public:
    class FieldIterator
    {
    public:
        FieldIterator() = default;

        FieldIterator(MonoClass* cl, void* iter)
            : m_iter(iter), m_class(cl)
        {
            m_field = mono_class_get_fields(m_class, &m_iter);
        }

        std::string operator*() const
        {
            return mono_field_get_name(m_field);
        }

        bool operator==(const FieldIterator& iter) const
        {
            return m_field == iter.m_field;
        }

        bool operator!=(const FieldIterator& iter) const
        {
            return !(*this == iter);
        }

        FieldIterator& operator++()
        {
            m_field = mono_class_get_fields(m_class, &m_iter);
            return *this;
        }

        FieldIterator operator++(int)
        {
            auto copy = *this;
            ++(*this);
            return copy;
        }
    
    private:
        void* m_iter = nullptr;
        MonoClass* m_class;
        MonoClassField* m_field;
    };

public:
    FieldView(MonoClass* type)
        : m_class(type) {}

    FieldIterator begin() const
    {
        return FieldIterator(m_class, nullptr);
    }

    FieldIterator end() const
    {
        return FieldIterator();
    }

    size_t size() const
    {
        return static_cast<size_t>(mono_class_num_fields(m_class));
    }
    
private:
    MonoClass* m_class;
};

}