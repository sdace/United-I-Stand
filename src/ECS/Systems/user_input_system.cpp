#include "ECS/Systems/user_input_system.hpp"

using namespace std;

void UserInputSystem::init()
{
    EntityQuery query =
        EntityQuery().all(1,
                          ComponentType<PlayerInputComponent>::get_id());
    m_entity_query = make_shared<EntityQuery>(move(query));
    add_query(m_entity_query);
}

void UserInputSystem::update(float elapsed_sec)
{
    auto entities = m_entity_query->get_entities();
    KeyboardService* service = KeyboardService::getInstance();

    for (const auto& e : entities)
    {
        PlayerInputComponent inputs = entity_manager->get_component_data<PlayerInputComponent>(e);
        // Update Component Data to match Keyboard Service

        // Pass values of Keyboard Service to component
        inputs.up_pressed    = service->isPressed(up_pressed);
        inputs.down_pressed  = service->isPressed(down_pressed);
        inputs.left_pressed  = service->isPressed(left_pressed);
        inputs.right_pressed = service->isPressed(right_pressed);
        inputs.one_pressed   = service->isPressed(one_pressed);
        inputs.two_pressed   = service->isPressed(two_pressed);
        inputs.three_pressed = service->isPressed(three_pressed);
        inputs.e_pressed     = service->isPressed(e_pressed);
        inputs.up_pressing    = service->isPressed(up_pressing);
        inputs.down_pressing  = service->isPressed(down_pressing);
        inputs.left_pressing  = service->isPressed(left_pressing);
        inputs.right_pressing = service->isPressed(right_pressing);
        inputs.one_pressing   = service->isPressed(one_pressing);
        inputs.two_pressing   = service->isPressed(two_pressing);
        inputs.three_pressing = service->isPressed(three_pressing);
        inputs.e_pressing     = service->isPressed(e_pressing);
        inputs.action_queued = service->is_action_queued();

        entity_manager->set_component_data(e, move(inputs));
    }

    resetKeyboard(service);

}

// reset keys_pressing
void UserInputSystem::resetKeyboard(KeyboardService* service)
{
    service->unpressKey(up_pressing);
    service->unpressKey(down_pressing);
    service->unpressKey(left_pressing);
    service->unpressKey(right_pressing);
    service->unpressKey(one_pressing);
    service->unpressKey(two_pressing);
    service->unpressKey(three_pressing);
    service->unpressKey(e_pressing);
}

