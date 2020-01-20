#include "ECS/Systems/render_system.hpp"

using namespace std;

void RenderSystem::init()
{
    // Regular entities
    EntityQuery query =
        EntityQuery().all(2,
            ComponentType<RenderableComponent>::get_id(),
            ComponentType<PositionComponent>::get_id())
            .none(1,ComponentType<MenuComponent>::get_id());

    m_entity_query = make_shared<EntityQuery>(move(query));
    add_query(m_entity_query);

    // MENU
    EntityQuery menu = EntityQuery().all(3,
        ComponentType<RenderableComponent>::get_id(),
        ComponentType<PositionComponent>::get_id(),
        ComponentType<MenuComponent>::get_id());

    m_menu_query = make_shared<EntityQuery>(move(menu));
    add_query(m_menu_query);

    // PAUSED
    EntityQuery paused = EntityQuery().all(1,
        ComponentType<PausedComponent>::get_id());

    m_paused_query = make_shared<EntityQuery>(move(paused));
    add_query(m_paused_query);

    // CONTROLS
    EntityQuery controls = EntityQuery().all(1,
        ComponentType<ControlsComponent>::get_id());

    m_controls_query = make_shared<EntityQuery>(move(controls));
    add_query(m_controls_query);

    // CREDITS
    EntityQuery credits = EntityQuery().all(1,
        ComponentType<CreditsComponent>::get_id());

    m_credits_query = make_shared<EntityQuery>(move(credits));
    add_query(m_credits_query);

    // CREDITS
    EntityQuery congrats = EntityQuery().all(1,
        ComponentType<CongratsComponent>::get_id());

    m_congrats_query = make_shared<EntityQuery>(move(congrats));
    add_query(m_congrats_query);

    EntityQuery player = EntityQuery().all(1,
        ComponentType<PlayerComponent>::get_id());

    m_player_query = make_shared<EntityQuery>(move(player));
    add_query(m_player_query);
}

void RenderSystem::update(float elapsed_sec)
{

    // Handle MENU 
    SceneService* instance = SceneService::getInstance();
    Scene scene = instance->getScene();

    Transform trans;
    if (scene != GAME)
    {
        for (const auto& e : m_player_query->get_entities())
        {
            trans = entity_manager->get_component_data<RenderableComponent>(e).transform;
        }
    }

    unordered_set<Entity> entities;
    switch (scene)
    {
        case MENU:
            entities = m_menu_query->get_entities();
            break;
        case GAME:
            entities = m_entity_query->get_entities();
            break;
        case PAUSED:
            entities = m_paused_query->get_entities();
            break;
        case CONTROLS:
            entities = m_controls_query->get_entities();
            break;
        case CREDITS:
            entities = m_credits_query->get_entities();
            break;
        case CONGRATS:
            entities = m_congrats_query->get_entities();
            break;
    }
    
    for (const auto& e : entities)
    {
        auto& gunit = GUnit::get_instance();

        RenderableComponent r = entity_manager->get_component_data<RenderableComponent>(e);
        PositionComponent position_component = entity_manager->get_component_data<PositionComponent>(e);
        if (entity_manager->has_component<FacingComponent>(e))
        {
            FacingComponent facing = entity_manager->get_component_data<FacingComponent>(e);
            gunit.draw(r.gstate, r.transform, position_component, facing.Value);
        }
        else
        {
            if (scene != GAME)
            {
                gunit.draw_menu(r.gstate);
            }
            else
            {
                gunit.draw(r.gstate, r.transform, position_component, FacingDirection::RIGHT);
            }
        }
    }
}
