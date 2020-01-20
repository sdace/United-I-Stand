// internal
#include "entity_query.hpp"

// stlib
#include <stdarg.h>

EntityQuery::EntityQuery() { }

EntityQuery EntityQuery::all(int n, ...)
{
    va_list args;
    va_start(args, n);
    
    for (int i = 0; i < n; i++)
    {
        m_all.set(va_arg(args, ComponentID));
    }

    va_end(args);

    return *this;
}

EntityQuery EntityQuery::none(int n, ...)
{
    va_list args;
    va_start(args, n);

    for (int i = 0; i < n; i++)
    {
        m_none.set(va_arg(args, ComponentID));
    }

    va_end(args);

    return *this;
}

EntityQuery EntityQuery::one(int n, ...)
{
    va_list args;
    va_start(args, n);

    for (int i = 0; i < n; i++)
    {
        m_one.set(va_arg(args, ComponentID));
    }

    va_end(args);

    return *this;
}

unordered_set<Entity> EntityQuery::get_entities()
{
    return m_entities;
}

void EntityQuery::update_query(Entity e, ComponentSet component_set)
{
    if (m_entities.find(e) != m_entities.end())
    {
        // Check if we're no longer interested
        if (!m_is_match(component_set))
        {
            m_entities.erase(e);
        }
    }
    else
    {
        // Check if we're interested
        if (m_is_match(component_set))
        {
            m_entities.insert(e);
        }
    }
}

bool EntityQuery::m_is_match(ComponentSet component_set)
{
    // fail if component set doesn't have all the required components
    if ((component_set & m_all) != m_all)
    {
        return false;
    }

    // fail if component set has one of the "none" components
    if (!(component_set & m_none).none())
    {
        return false;
    }

    // fail if 'one' components are specified, and it doesn't have one
    if (m_one.none())
    {
        return true;
    }
    else
    {
        return !(component_set & m_one).none();
    }
}
