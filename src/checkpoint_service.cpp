#include "checkpoint_service.hpp"

#include <algorithm>

CheckpointService& CheckpointService::get_instance()
{
    static CheckpointService instance;
    return instance;
}

CheckpointService::CheckpointService() {}

Entity CheckpointService::last_checkpoint()
{
    return m_last_checkpoint;
}

vec2 CheckpointService::get_starting_position()
{
    return m_starting_position;
}

void CheckpointService::set_starting_position(vec2 position)
{
    m_starting_position = position;
}

void CheckpointService::next_checkpoint()
{
    if (!m_entity_manager->is_valid_entity(m_last_checkpoint))
    {
        set_checkpoint(0);
        return;
    }

    auto checkpoint = m_entity_manager->get_component_data<CheckpointComponent>(m_last_checkpoint);
    int current_id = checkpoint.id;
    int closest_id = 999999;

    auto checkpoints = m_checkpoint_query->get_entities();
    float closest_difference = INFINITY;
    for (const auto& c : checkpoints)
    {
        auto check_comp = m_entity_manager->get_component_data<CheckpointComponent>(c);
        if ((check_comp.id > current_id) && (check_comp.id - current_id < closest_difference))
        {
            closest_difference = check_comp.id - current_id;
            closest_id = check_comp.id;
        }
    }

    if (closest_id != current_id && closest_id != INFINITY)
    {
        set_checkpoint(closest_id);
    }
}

void CheckpointService::prev_checkpoint()
{
    if (!m_entity_manager->is_valid_entity(m_last_checkpoint))
    {
        set_checkpoint(0);
        return;
    }

    auto checkpoint = m_entity_manager->get_component_data<CheckpointComponent>(m_last_checkpoint);
    int current_id = checkpoint.id;
    int closest_id = -999999;

    auto checkpoints = m_checkpoint_query->get_entities();
    float closest_difference = -INFINITY;
    for (const auto& c : checkpoints)
    {
        auto check_comp = m_entity_manager->get_component_data<CheckpointComponent>(c);
        if ((check_comp.id < current_id) && (check_comp.id - current_id > closest_difference))
        {
            closest_difference = check_comp.id - current_id;
            closest_id = check_comp.id;
        }
    }

    if (closest_id != current_id && closest_id != -INFINITY)
    {
        set_checkpoint(closest_id);
    }
}

bool CheckpointService::set_checkpoint(int id)
{
    auto checkpoints = m_checkpoint_query->get_entities();
    for (const auto& c : checkpoints)
    {
        auto check_comp = m_entity_manager->get_component_data<CheckpointComponent>(c);
        if (check_comp.id == id)
        {
            m_event_manager->post<ActivateCheckpointEvent>(ActivateCheckpointEvent(c));
            m_event_manager->post<PlayerDeathEvent>(PlayerDeathEvent());
            return true;
        }
    }
    return false;
}

void CheckpointService::init(shared_ptr<EntityManager> entity_manager, shared_ptr<EventManager> event_manager)
{
    m_last_checkpoint = Entity::NULL_ENTITY;
    m_entity_manager = entity_manager;
    m_event_manager = event_manager;
    SoundPlayer::get_instance().loadsound("checkpoint.wav");

    EntityQuery checkpoint_query =
        EntityQuery().all(1,
                          ComponentType<CheckpointComponent>::get_id());
    m_checkpoint_query = make_shared<EntityQuery>(move(checkpoint_query));
    m_entity_manager->register_query(m_checkpoint_query);

    auto activate_checkpoint_handler = bind(&CheckpointService::activate_checkpoint_handler, this, placeholders::_1);
    event_manager->add_handler<ActivateCheckpointEvent>(activate_checkpoint_handler);

    load_checkpoints();
}

void CheckpointService::load_checkpoints()
{
    pugi::xml_parse_result result = doc.load_file(PATH_TMX);
    if (!result)
    {
        cout << "CHECKPOINTSERVICE::INIT: fail to load the tmx file!" << endl;
        return;
    }

    auto object_groups = doc.child("map").children("objectgroup");
    pugi::xml_node checkpoints;

    for (auto& og : object_groups)
    {
        auto name = og.attribute("name");
        if (strcmp(name.as_string(), "checkpoints") == 0)
        {
            checkpoints = og;
            break;
        }
    }

    for (auto& c : checkpoints.children("object"))
    {
        float x = c.attribute("x").as_float() + TILE_SIZE / 2;
        float y = c.attribute("y").as_float() + TILE_SIZE / 2;

        Entity checkpoint = m_entity_manager->create_entity();

        int checkpoint_id = 0;

        for (auto& p : c.child("properties").children("property"))
        {
            auto name = p.attribute("name").as_string();
            auto value = p.attribute("value");
            if (strcmp(name, "id") == 0)
            {
                checkpoint_id = value.as_int();
            }
        }

        m_entity_manager->add_component_data(checkpoint, move(PositionComponent({ x, y })));
        m_entity_manager->add_component_data(checkpoint, move(CheckpointComponent(checkpoint_id)));
        m_entity_manager->add_component_data(checkpoint, move(RenderableComponent("inactive_checkpoint")));
        m_entity_manager->add_component_data(checkpoint, move(AABBComponent({ 0.f, 0.f }, { 16.f, 16.f })));
    }
}

void CheckpointService::activate_checkpoint_handler(ActivateCheckpointEvent ev)
{
    SoundPlayer::get_instance().playsound("checkpoint.wav");

    Entity activated_checkpoint = ev.activated_checkpoint;

    RenderableComponent checkpoint_renderable = m_entity_manager->get_component_data<RenderableComponent>(activated_checkpoint);
    checkpoint_renderable.gstate = "active_checkpoint";
    m_entity_manager->set_component_data(activated_checkpoint, move(checkpoint_renderable));

    CheckpointComponent activated_checkpoint_comp = m_entity_manager->get_component_data<CheckpointComponent>(activated_checkpoint);
    activated_checkpoint_comp.Activated = true;
    m_entity_manager->set_component_data(activated_checkpoint, move(activated_checkpoint_comp));

    m_last_checkpoint = activated_checkpoint;

    auto checkpoints = m_checkpoint_query->get_entities();
    for (const auto& c : checkpoints)
    {
        if (activated_checkpoint == c)
        {
            continue;
        }

        RenderableComponent inactive_checkpoint_renderable = m_entity_manager->get_component_data<RenderableComponent>(c);
        inactive_checkpoint_renderable.gstate = "inactive_checkpoint";
        m_entity_manager->set_component_data(c, move(inactive_checkpoint_renderable));

        CheckpointComponent inactive_checkpoint = m_entity_manager->get_component_data<CheckpointComponent>(c);
        inactive_checkpoint.Activated = false;
        m_entity_manager->set_component_data(c, move(inactive_checkpoint));
    }
}