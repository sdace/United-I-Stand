#pragma once

#include "keyboard_service.hpp"
#include "ECS/Components/player_input_component.hpp"
#include "ECS/entity_manager.hpp"

class UserInputSystem : public EntitySystem
{
public:
    void init() override;
    void update(float elapsed_sec) override;

private:
    shared_ptr<EntityQuery> m_entity_query;

    void resetKeyboard(KeyboardService* service);
};
