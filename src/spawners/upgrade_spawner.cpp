#include "upgrade_spawner.hpp"

void UpgradeSpawner::init(shared_ptr<EntityManager> entity_manager, shared_ptr<EventManager> event_manager)
{
    pugi::xml_parse_result result = doc.load_file(PATH_TMX);
    if (!result)
    {
        cout << "fail to load the tmx file!" << endl;
        return;
    }

    auto object_groups = doc.child("map").children("objectgroup");
    pugi::xml_node platforms;
    for (auto& og : object_groups)
    {
        auto name = og.attribute("name");
        if (strcmp(name.as_string(), "upgrades") == 0)
        {
            platforms = og;
            break;
        }
    }

    for (auto& e : platforms.children("object"))
    {
        UpgradeType upgrade_type;
        float x = e.attribute("x").as_float() + TILE_SIZE / 2.f;
        float y = e.attribute("y").as_float() + TILE_SIZE / 2.f;
        for (auto& p : e.child("properties").children("property"))
        {
            auto name = p.attribute("name").as_string();
            auto value = p.attribute("value");
            if (strcmp(name, "upgrade_type") == 0)
            {
                if (name)
                if (strcmp(value.as_string(), "spartan") == 0)
                {
                    upgrade_type = UpgradeType::SPARTAN_UPGRADE;
                }
                else if (strcmp(value.as_string(), "wheel") == 0)
                {
                    upgrade_type = UpgradeType::WHEEL_UPGRADE;
                }
            }
        }
        make_upgrade(entity_manager, upgrade_type, x, y);
    }
}

void UpgradeSpawner::update(float elapsed_time) {}

void UpgradeSpawner::make_upgrade(std::shared_ptr<EntityManager> entity_manager, UpgradeType upgrade_type, float x, float y)
{
    Entity e = entity_manager->create_entity();
    PositionComponent pos = PositionComponent({ x, y });
    entity_manager->add_component_data(e, move(pos));
    AABBComponent aabb({ 0.f, 0.f }, { 16.f, 16.f });
    entity_manager->add_component_data(e, move(aabb));
    UpgradeComponent upgrade_component = UpgradeComponent(upgrade_type);
    entity_manager->add_component_data(e, move(upgrade_component));

    if (upgrade_type == UpgradeType::SPARTAN_UPGRADE)
    {
        RenderableComponent renderable_component = RenderableComponent("spartan_idle_spear");
        entity_manager->add_component_data(e, move(renderable_component));
    }
    else if (upgrade_type == UpgradeType::WHEEL_UPGRADE)
    {
        RenderableComponent renderable_component = RenderableComponent("wheel_idle");
        entity_manager->add_component_data(e, move(renderable_component));
    }
}