#pragma once

// internal
#include "component_array.hpp"
#include "entity_system.hpp"

// stlib
#include <unordered_map>
#include <vector>
#include <bitset>
#include <stack>
#include <typeindex>
#include <algorithm>
#include <unordered_set>
#include <assert.h>

using namespace std;

class EntityManager
{
public:
    EntityManager()
    {
        m_last_entity = Entity{ 1, 1 };
    }

    Entity create_entity()
    {
        Entity entity;

        // We use the entity pool to reuse entity IDs that were previously discarded
        if (m_entity_pool.size() > 0)
        {
            entity = m_entity_pool.top();
            m_entity_pool.pop();
            entity.Version++;
        }
        else
        {
            entity = Entity{ m_last_entity.Id++, 1 };
        }
        m_entities.insert(entity);

        m_component_sets[entity];

        return entity;
    }

    void destroy_entity(Entity e)
    {
        assert(is_valid_entity(e));

        for (auto const& pair : m_component_store)
        {
            auto component_array = pair.second;
            component_array->destroy_entity(e);
        }

        m_entities.erase(e);
        m_entity_pool.push(e);

        // Remove all components from unused entity ID
        m_component_sets[e].reset(); 

        // Update system
        for (auto system : m_systems)
        {
            system->update_queries(e, m_component_sets[e]);
        }
        for (auto query : m_queries)
        {
            query->update_query(e, m_component_sets[e]);
        }

        m_component_sets.erase(e);
    }

    template <typename T>
    void remove_component(Entity e)
    {
        assert(is_valid_entity(e));

        ComponentID component_id = ComponentType<T>::get_id();

        // Update component array
        auto component_array = static_pointer_cast<ComponentArray<T>>(m_component_store[component_id]);
        component_array->remove_data(e);

        // Update bitset
        m_component_sets[e].set(component_id, false);

        // Update system
        for (auto system : m_systems)
        {
            system->update_queries(e, m_component_sets[e]);
        }
        for (auto query : m_queries)
        {
            query->update_query(e, m_component_sets[e]);
        }
    }

    template <typename T>
    T get_component_data(Entity e)
    {
        assert(is_valid_entity(e));

        ComponentID component_id = ComponentType<T>::get_id();
        auto component_array = static_pointer_cast<ComponentArray<T>>(m_component_store[component_id]);
        return component_array->get_data(e);
    }

    template <typename T>
    bool has_component(Entity e)
    {
        assert(is_valid_entity(e));

        ComponentID component_id = ComponentType<T>::get_id();
        if (m_component_store.find(component_id) == m_component_store.end())
        {
            return false;
        }
        auto component_array = static_pointer_cast<ComponentArray<T>>(m_component_store[component_id]);
        return component_array->has_data(e);
    }

    template <typename T>
    void add_component_data(Entity e, T&& component)
    {
        assert(is_valid_entity(e));

        ComponentID component_id = ComponentType<T>::get_id();

        // Initialize component array if not already done
        if (!m_component_store[component_id])
        {
            m_component_store[component_id] = make_shared<ComponentArray<T>>();
        }

        // Add new component to our component store
        auto component_array = static_pointer_cast<ComponentArray<T>>(m_component_store[component_id]);
        component_array->add_data(e, move(component));

        // Update bitset
        m_component_sets[e].set(ComponentType<T>::get_id(), true);

        // Update system
        for (auto system : m_systems)
        {
            system->update_queries(e, m_component_sets[e]);
        }
        for (auto query : m_queries)
        {
            query->update_query(e, m_component_sets[e]);
        }
    }

    template <typename T>
    void set_component_data(Entity e, T&& component)
    {
        assert(is_valid_entity(e));

        ComponentID component_id = ComponentType<T>::get_id();

        auto component_array = static_pointer_cast<ComponentArray<T>>(m_component_store[component_id]);
        component_array->set_data(e, move(component));
    }

    bool is_valid_entity(Entity e)
    {
        return (m_entities.find(e) != m_entities.end());
    }

    void register_system(shared_ptr<EntitySystem> system)
    {
        m_systems.push_back(system);
    }

    void register_query(shared_ptr<EntityQuery> query)
    {
        m_queries.push_back(query);
    }

private:
    unordered_set<Entity> m_entities;
    unordered_map<Entity, ComponentSet> m_component_sets;

    unordered_map<ComponentID, shared_ptr<IComponentArray>> m_component_store;

    vector<shared_ptr<EntitySystem>> m_systems;

    Entity m_last_entity;
    stack<Entity> m_entity_pool; // for entity id recycling

    vector<shared_ptr<EntityQuery>> m_queries;
};
