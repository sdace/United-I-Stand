#include "ECS/entity_system.hpp"

#include <memory>

using namespace std;

void EntitySystem::setup(shared_ptr<EntityManager> em, shared_ptr<EventManager> ev, bool can_be_paused)
{
    entity_manager = em;
    event_manager = ev;
    this->can_be_paused = can_be_paused;
    init();
}

void EntitySystem::add_query(shared_ptr<EntityQuery> query)
{
    m_queries.push_back(query);
}

void EntitySystem::update_queries(Entity e, ComponentSet component_set)
{
    for (auto it = m_queries.begin(); it != m_queries.end(); ++it)
    {
        auto query = *it;
        query->update_query(e, component_set);
    }
}
