#include "Scene.h"

#ifndef LEVEL2_H
#define LEVEL2_H

constexpr int LEVEL2_WIDTH = 30,
              LEVEL2_HEIGHT = 10;

class Level2 : public Scene {
private:
    unsigned int mLevelData[LEVEL2_WIDTH * LEVEL2_HEIGHT];
    Entity *mCat;

public:
    static constexpr float TILE_DIMENSION       = 75.0f,
                        ACCELERATION_OF_GRAVITY = 981.0f;

    Level2();
    Level2(Vector2 origin, const char *bgHexCode);
    ~Level2();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif

