#pragma once

typedef unsigned int EventID;

class IEventType
{
protected:
    static EventID m_last_id;
};

template <typename T>
class EvType : public IEventType
{
public:
    static EventID get_id()
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
    static EventID m_id;
    static bool m_has_id;
};

template <typename T>
EventID EvType<T>::m_id = 0;

template <typename T>
bool EvType<T>::m_has_id = false;
