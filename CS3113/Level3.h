#include "Scene.h"

#ifndef LEVEL3_H
#define LEVEL3_H

constexpr int LEVEL3_WIDTH = 30,
              LEVEL3_HEIGHT = 10;

class Level3 : public Scene {
private:
    unsigned int mLevelData[LEVEL3_WIDTH * LEVEL3_HEIGHT];
    Entity *mDemon;

public:
    static constexpr float TILE_DIMENSION       = 75.0f,
                        ACCELERATION_OF_GRAVITY = 981.0f;

    Level3();
    Level3(Vector2 origin, const char *bgHexCode);
    ~Level3();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif

