#include "scene_service.hpp"

SceneService* SceneService::instance = 0;

Scene scene;

SceneService* SceneService::getInstance()
{
    if(!instance)
    {
        instance = new SceneService;
    }
    return instance;
}

Scene SceneService::getScene()
{
    return scene;
}

void SceneService::setScene(Scene newScene)
{
    scene = newScene;
}
