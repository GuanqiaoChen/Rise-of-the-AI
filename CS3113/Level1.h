#include "Scene.h"

#ifndef LEVEL1_H
#define LEVEL1_H

constexpr int LEVEL1_WIDTH = 30,
              LEVEL1_HEIGHT = 10;

class Level1 : public Scene {
private:
    unsigned int mLevelData[LEVEL1_WIDTH * LEVEL1_HEIGHT];
    Entity *mSkeleton;

public:
    static constexpr float TILE_DIMENSION       = 75.0f,
                        ACCELERATION_OF_GRAVITY = 981.0f;

    Level1();
    Level1(Vector2 origin, const char *bgHexCode);
    ~Level1();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif

