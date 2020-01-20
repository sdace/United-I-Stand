#pragma once

// Define possible scenes
enum Scene
{
    MENU,
    GAME,
    PAUSED,
    CONTROLS,
    CREDITS,
    CONGRATS
};

// Singleton service that stores scene
class SceneService
{
private:
    static SceneService* instance;
    
    SceneService()
    {
        scene = MENU;
    };

    Scene scene;

public:
    static SceneService* getInstance();
    Scene getScene();
    void setScene(Scene newScene);
};
