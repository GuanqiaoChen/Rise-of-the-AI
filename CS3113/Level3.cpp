/**
* Author: Guanqiao Chen
* Assignment: Rise of the AI
* Date due: 2025-11-08, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Level3.h"

Level3::Level3() : Scene { {0.0f}, nullptr } {}
Level3::Level3(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Level3::~Level3() { shutdown(); }

void Level3::initialise()
{
    mGameState.nextSceneID = 0;

    // To avoid memory corruption issues
    auto setSpan = [&](int row, int c0, int c1, int tileId) {
        if (LEVEL3_WIDTH <= 0 || LEVEL3_HEIGHT <= 0) return;
        row = std::max(0, std::min(LEVEL3_HEIGHT - 1, row));
        if (c0 > c1) std::swap(c0, c1);
        c0 = std::max(0, c0);
        c1 = std::min(LEVEL3_WIDTH - 1, c1);
        for (int c = c0; c <= c1; ++c)
            mLevelData[row * LEVEL3_WIDTH + c] = tileId;
    };

    for (int i = 0; i < LEVEL3_WIDTH * LEVEL3_HEIGHT; ++i) mLevelData[i] = 0;

    if (LEVEL3_WIDTH > 0 && LEVEL3_HEIGHT > 1) {
        setSpan(LEVEL3_HEIGHT - 1, 0, LEVEL3_WIDTH - 1, 20); 
        setSpan(LEVEL3_HEIGHT - 2, 0, LEVEL3_WIDTH - 1, 21); 
    }

    {
        int y  = std::max(0, LEVEL3_HEIGHT - 7);
        int w  = std::max(3, LEVEL3_WIDTH / 6);
        int x0 = LEVEL3_WIDTH / 8;
        setSpan(y, x0, x0 + w, 22);
    }

    {
        int y  = std::max(0, LEVEL3_HEIGHT - 6);
        int w  = std::max(4, LEVEL3_WIDTH / 5);
        int x0 = std::max(0, (LEVEL3_WIDTH / 2) - (w / 2));
        setSpan(y, x0, x0 + w, 24);
    }

    {
        int y  = std::max(0, LEVEL3_HEIGHT - 9);
        int w  = std::max(3, LEVEL3_WIDTH / 8);
        int x0 = (LEVEL3_WIDTH * 3) / 4;
        setSpan(y, x0, x0 + w, 26);
    }

    mGameState.bgm = LoadMusicStream("assets/game/looped_background_music.wav");
    SetMusicVolume(mGameState.bgm, 0.5f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/player_jump_sound.wav");

    mGameState.map = new Map(
        LEVEL3_WIDTH, LEVEL3_HEIGHT,
        (unsigned int *) mLevelData,
        "assets/game/tilemap.png",
        TILE_DIMENSION,
        16, 7,  
        mOrigin
    );

    std::map<Direction, std::vector<int>> adventurerAnimationAtlas = {
        {LEFT,  {0, 1, 2, 3, 4, 5, 6, 7}},  
        {RIGHT, {0, 1, 2, 3, 4, 5, 6, 7}}  
    };

    mGameState.xochitl = new Entity(
        {mOrigin.x - 500.0f, mOrigin.y - 150.0f},
        {160.0f, 160.0f},
        "assets/game/adventurer_walk_right.png",
        PLAYER
    );
    mGameState.xochitl->setLeftRightTextures(
        "assets/game/adventurer_walk_left.png",
        "assets/game/adventurer_walk_right.png",
        ATLAS,
        {1.0f, 8.0f}  
    );
    mGameState.xochitl->setAnimationAtlas(adventurerAnimationAtlas);
    mGameState.xochitl->setDirection(RIGHT); 
    mGameState.xochitl->setFrameSpeed(Entity::DEFAULT_FRAME_SPEED);
    mGameState.xochitl->setJumpingPower(550.0f);
    mGameState.xochitl->setColliderDimensions({80.0f, 80.0f});
    mGameState.xochitl->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.xochitl->setSpeed(200);

    std::map<Direction, std::vector<int>> demonAnimationAtlas = {
        {LEFT,  {0, 1, 2, 3}},  
        {RIGHT, {0, 1, 2, 3}}  
    };

    mDemon = new Entity(
        {mOrigin.x + 150.0f, mOrigin.y - 200.0f},
        {80.0f, 80.0f},
        "assets/game/demon_fly_right.png",
        NPC
    );
    mDemon->setLeftRightTextures(
        "assets/game/demon_fly_left.png",
        "assets/game/demon_fly_right.png",
        ATLAS,
        {1.0f, 4.0f}  
    );
    mDemon->setAnimationAtlas(demonAnimationAtlas);
    mDemon->setDirection(RIGHT); 
    mDemon->setFrameSpeed(Entity::DEFAULT_FRAME_SPEED);
    mDemon->setAIType(FLYER);
    mDemon->setAIState(WALKING);
    mDemon->setSpeed(120);
    mDemon->setAcceleration({0.0f, 0.0f});
    mDemon->setColliderDimensions({50.0f, 50.0f});

    mGameState.camera = { 0 };
    mGameState.camera.target = mGameState.xochitl->getPosition();
    mGameState.camera.offset = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;
}

void Level3::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    // Update demon AI
    mDemon->update(
        deltaTime,
        mGameState.xochitl,
        mGameState.map,
        nullptr,
        0
    );

    // Update player
    mGameState.xochitl->update(
        deltaTime,
        nullptr,
        mGameState.map,
        nullptr,
        0
    );

    // Check collision with AI 
    if (mGameState.xochitl->checkCollisionWithAI(mDemon))
    {
        // Player loses a life
        mGameState.nextSceneID = -1; 
    }

    // Check if player reached end of level 
    if (mGameState.xochitl->getPosition().x > mOrigin.x + 800.0f)
    {
        mGameState.nextSceneID = -2;
    }

    // Camera follows player
    Vector2 currentPlayerPosition = { mGameState.xochitl->getPosition().x, mOrigin.y };
    panCamera(&mGameState.camera, &currentPlayerPosition);
}

void Level3::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mDemon->render();
    mGameState.xochitl->render();
}

void Level3::shutdown()
{
    delete mGameState.xochitl;
    delete mGameState.map;
    delete mDemon;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
}

