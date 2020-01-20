#pragma once

class EntityManager;

#include "ECS/entity_query.hpp"
#include "event_manager.hpp"

// stlib
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

class EntitySystem
{
public:
    void setup(shared_ptr<EntityManager> entity_manager, shared_ptr<EventManager> event_manager, bool can_be_paused);
    virtual void init() = 0;
    virtual void update(float elapsed_sec) = 0;
    void add_query(shared_ptr<EntityQuery> query);
    void update_queries(Entity e, ComponentSet component_set);
    bool can_be_paused;

private:
    vector<shared_ptr<EntityQuery>> m_queries;

protected:
    shared_ptr<EntityManager> entity_manager;
    shared_ptr<EventManager> event_manager;
};
