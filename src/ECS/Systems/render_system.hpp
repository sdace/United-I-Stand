#pragma once

#include "ECS/Components/renderable_component.hpp"
#include "ECS/Components/position_component.hpp"
#include "ECS/Components/velocity_component.hpp"
#include "ECS/Components/menu_component.hpp"
#include "ECS/Components/paused_component.hpp"
#include "ECS/Components/controls_component.hpp"
#include "ECS/Components/credits_component.hpp"
#include "ECS/Components/player_component.hpp"
#include "ECS/Components/congrats_component.hpp"
#include "ECS/entity_manager.hpp"
#include "common.hpp"
#include "G-Unit/g_unit.hpp"
#include "scene_service.hpp"

class RenderSystem : public EntitySystem
{
public:
    void init() override;
    void update(float elapsed_sec) override;

private:
    shared_ptr<EntityQuery> m_entity_query;
    shared_ptr<EntityQuery> m_menu_query;
    shared_ptr<EntityQuery> m_paused_query;
    shared_ptr<EntityQuery> m_controls_query;
    shared_ptr<EntityQuery> m_credits_query;
    shared_ptr<EntityQuery> m_congrats_query;

    shared_ptr<EntityQuery> m_player_query;
};
