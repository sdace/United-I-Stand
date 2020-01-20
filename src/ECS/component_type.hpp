#pragma once

typedef unsigned int ComponentID;

class IComponentType
{
protected:
    static ComponentID m_last_id;
};

template <typename T>
class ComponentType : public IComponentType
{
public:
    static ComponentID get_id()
    {
        if (!m_has_id)
        {
            m_id = m_last_id;
            m_last_id++;
            m_has_id = true;
        }

        return m_id;
    }

private:
    static ComponentID m_id;
    static bool m_has_id;
};

template <typename T>
ComponentID ComponentType<T>::m_id = 0;

template <typename T>
bool ComponentType<T>::m_has_id = false;