#include "menu_spawner.hpp"

void MenuSpawner::init(shared_ptr<EntityManager> entity_manager)
{
    // SPAWN ALL MENUS
    Entity menu = entity_manager->create_entity();
    Entity paused = entity_manager->create_entity();
    Entity controls = entity_manager->create_entity();
    Entity credits = entity_manager->create_entity();
    Entity congrats = entity_manager->create_entity();
    PositionComponent menu_pos({650, 910});
    entity_manager->add_component_data(menu, move(menu_pos));
    entity_manager->add_component_data(paused, move(menu_pos));
    entity_manager->add_component_data(controls, move(menu_pos));
    entity_manager->add_component_data(credits, move(menu_pos));
    entity_manager->add_component_data(congrats, move(menu_pos));
    RenderableComponent menu_r = RenderableComponent("menu");
    RenderableComponent paused_r = RenderableComponent("paused");
    RenderableComponent controls_r = RenderableComponent("controls");
    RenderableComponent credits_r = RenderableComponent("credits");
    RenderableComponent congrats_r = RenderableComponent("congratulations");
    entity_manager->add_component_data(menu, move(menu_r));
    entity_manager->add_component_data(paused, move(paused_r));
    entity_manager->add_component_data(controls, move(controls_r));
    entity_manager->add_component_data(credits, move(credits_r));
    entity_manager->add_component_data(congrats, move(congrats_r));
    MenuComponent menu_component = MenuComponent();
    entity_manager->add_component_data(menu, move(menu_component));
    PausedComponent paused_component = PausedComponent();
    entity_manager->add_component_data(paused, move(paused_component));
    ControlsComponent controls_component = ControlsComponent();
    entity_manager->add_component_data(controls, move(controls_component));
    CreditsComponent credits_component = CreditsComponent();
    entity_manager->add_component_data(credits, move(credits_component));

    CongratsComponent congrats_component = CongratsComponent();
    entity_manager->add_component_data(congrats, move(congrats_component));
}
void MenuSpawner::update(float elapsed_time)
{

}
