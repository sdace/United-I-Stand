#include "player_spawner.hpp"

void PlayerSpawner::init(shared_ptr<EntityManager> entity_manager, shared_ptr<EventManager> event_manager)
{
    pugi::xml_parse_result result = doc.load_file(PATH_TMX);
    if (!result)
    {
        cout << "fail to load the tmx file!" << endl;
        return;
    }

    auto object_groups = doc.child("map").children("objectgroup");
    pugi::xml_node player;
    for (auto& og : object_groups)
    {
        auto name = og.attribute("name");
        if (strcmp(name.as_string(), "player") == 0)
        {
            player = og;
            break;
        }
    }

    for (auto& e : player.children("object"))
    {
        PlayerSpawnInfo playerSpawnInfo;
        playerSpawnInfo.position.x = e.attribute("x").as_float() + TILE_SIZE / 2;
        playerSpawnInfo.position.y = e.attribute("y").as_float() + TILE_SIZE / 2;
        playerSpawnInfo.type = e.attribute("type").as_string();
        for (auto& p : e.child("properties").children("property"))
        {
            auto name = p.attribute("name").as_string();
            auto value = p.attribute("value");
            if (strcmp(name, "facing_direction") == 0)
            {
                playerSpawnInfo.facing_direction = value.as_string();
            }
        }
        make_player(entity_manager, playerSpawnInfo);
        CheckpointService::get_instance().set_starting_position(playerSpawnInfo.position);

        RoomService& rs = RoomService::get_instance();
        int room_id = rs.room_id(playerSpawnInfo.position);
        assert(room_id != -1);
        Room starting_room = rs.get_room(room_id);
        rs.init_rooms(starting_room);
    }
}

void PlayerSpawner::update(float elapsed_time)
{

}

void PlayerSpawner::make_player(std::shared_ptr<EntityManager> entity_manager, PlayerSpawnInfo p)
{
    Entity e = entity_manager->create_entity();
    // Game logic stuff:
    PositionComponent pos(p.position);
    entity_manager->add_component_data(e, move(pos));
    VelocityComponent vel({ 0.0, 0.0 }, { 6.0, 13.0 });
    entity_manager->add_component_data(e, move(vel));
    if (p.facing_direction == "left")
    {
        FacingComponent facing = FacingComponent(FacingDirection::LEFT);
        entity_manager->add_component_data(e, move(facing));
    }
    else
    {
        FacingComponent facing = FacingComponent(FacingDirection::RIGHT);
        entity_manager->add_component_data(e, move(facing));
    }
    GravityComponent gravity{};
    entity_manager->add_component_data(e, move(gravity));
    PlayerComponent pcomp{};
    entity_manager->add_component_data(e, move(pcomp));
    PlayerInputComponent inputs{};
    entity_manager->add_component_data(e, move(inputs));
    AABBComponent aabb({ 0.f, 0.f }, { 16.f, 16.f });
    entity_manager->add_component_data(e, move(aabb));
    TileColliderComponent tile_collider = TileColliderComponent();
    entity_manager->add_component_data(e, move(tile_collider));
    ColliderComponent collider = ColliderComponent();
    entity_manager->add_component_data(e, move(collider));
    RenderableComponent renderable_component = RenderableComponent("player_idle");
    entity_manager->add_component_data(e, move(renderable_component));
}