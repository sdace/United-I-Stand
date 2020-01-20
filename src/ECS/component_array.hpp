#pragma once

// internal
#include "ECS/entity.hpp"

// stlib
#include <unordered_map>
#include <vector>
#include <assert.h>

using namespace std;

// Useful interface for static pointer casting, as we can't down-cast a general
// component vector to a vector of a specific component type
// This implementation of ComponentArray is inspired by https://austinmorlan.com/posts/entity_component_system/

class IComponentArray
{
public:
    virtual void destroy_entity(Entity e) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray
{
public:

    void add_data(Entity e, T&& component_data)
    {
        assert(m_entity_index_map.find(e) == m_entity_index_map.end());

        m_components.push_back(component_data);
        size_t index = m_components.size() - 1;
        m_entity_index_map[e] = index;
        m_index_entity_map[index] = e;
    }

    void remove_data(Entity e)
    {
        size_t index = m_entity_index_map[e];
        size_t last_index = m_components.size() - 1;

        if (index != last_index)
        {
            // We swap elements to keep the array packed
            iter_swap(m_components.begin() + index, m_components.end() - 1);
            Entity last_entity = m_index_entity_map[last_index];
            m_entity_index_map[last_entity] = index;
            m_index_entity_map[index] = last_entity;
        }

        m_components.pop_back();

        // Erase invalid entries in map
        m_entity_index_map.erase(e);
        m_index_entity_map.erase(last_index);
    }

    T get_data(Entity e)
    {
        assert(m_entity_index_map.find(e) != m_entity_index_map.end());
        return m_components[m_entity_index_map[e]];
    }

    bool has_data(Entity e)
    {
        return (m_entity_index_map.find(e) != m_entity_index_map.end());
    }

    void set_data(Entity e, T&& component_data)
    {
        assert(m_entity_index_map.find(e) != m_entity_index_map.end());

        size_t index = m_entity_index_map[e];
        m_components[index] = component_data;
    }

    void destroy_entity(Entity e) override
    {
        if (m_entity_index_map.find(e) != m_entity_index_map.end())
        {
            remove_data(e);
        }
    }

private:

    unordered_map<size_t, Entity> m_index_entity_map;
    unordered_map<Entity, size_t> m_entity_index_map;
    vector<T> m_components;
};
