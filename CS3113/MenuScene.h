#include "Scene.h"

#ifndef MENUSCENE_H
#define MENUSCENE_H

class MenuScene : public Scene {
public:
    MenuScene();
    MenuScene(Vector2 origin, const char *bgHexCode);
    ~MenuScene();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif

