#include "platform_spawner.hpp"

void PlatformSpawner::init(shared_ptr<EntityManager> entity_manager, shared_ptr<EventManager> event_manager)
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
        if (strcmp(name.as_string(), "platforms") == 0)
        {
            platforms = og;
            break;
        }
    }

    for (auto& e : platforms.children("object"))
    {
        PlatformSpawnInfo platformSpawnInfo;
        platformSpawnInfo.position.x = e.attribute("x").as_float() + TILE_SIZE / 2.f;
        platformSpawnInfo.position.y = e.attribute("y").as_float() + TILE_SIZE / 2.f;
        platformSpawnInfo.type = e.attribute("type").as_string();
        for (auto& p : e.child("properties").children("property"))
        {
            auto name = p.attribute("name").as_string();
            auto value = p.attribute("value");
            if (strcmp(name, "is_circular") == 0)
            {
                if (strcmp(value.as_string(), "true") == 0)
                {
                    platformSpawnInfo.is_circular = true;
                }
                else
                {
                    platformSpawnInfo.is_circular = false;
                }
            }
            if (strcmp(name, "speed") == 0)
            {
                platformSpawnInfo.speed = value.as_float();
            }
            if (strcmp(name, "displacement_x") == 0)
            {
                platformSpawnInfo.displacement.x = value.as_float();
            }
            if (strcmp(name, "displacement_y") == 0)
            {
                platformSpawnInfo.displacement.y = value.as_float();
            }
        }
        make_platform(entity_manager, platformSpawnInfo);
    }
}

void PlatformSpawner::update(float elapsed_time)
{

}

void PlatformSpawner::make_platform(std::shared_ptr<EntityManager> entity_manager, PlatformSpawnInfo p)
{
    Entity e = entity_manager->create_entity();
    PositionComponent pos(p.position);
    entity_manager->add_component_data(e, move(pos));
    VelocityComponent vel({ -0.3, 0.3 }, { 1.0, 1.0 });
    entity_manager->add_component_data(e, move(vel));
    SolidComponent solid = SolidComponent(true);
    entity_manager->add_component_data(e, move(solid));
    AABBComponent aabb({ 0.f, 0.f }, { 32.f, 16.f });
    entity_manager->add_component_data(e, move(aabb));
    OscillatorComponent oc = OscillatorComponent(p.position, p.displacement, p.speed, p.is_circular);
    entity_manager->add_component_data(e, move(oc));
    RenderableComponent renderable_component = RenderableComponent(p.type);
    entity_manager->add_component_data(e, move(renderable_component));
}