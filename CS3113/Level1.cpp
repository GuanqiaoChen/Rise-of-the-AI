/**
* Author: Guanqiao Chen
* Assignment: Rise of the AI
* Date due: 2025-11-08, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Level1.h"

Level1::Level1() : Scene { {0.0f}, nullptr } {}
Level1::Level1(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Level1::~Level1() { shutdown(); }

void Level1::initialise()
{
    mGameState.nextSceneID = 0;

    // Create level data
    for (int i = 0; i < LEVEL1_WIDTH * LEVEL1_HEIGHT; i++) mLevelData[i] = 0;

    for (int c = 0; c < LEVEL1_WIDTH; ++c) {
        mLevelData[(LEVEL1_HEIGHT - 1) * LEVEL1_WIDTH + c] = 1;
        mLevelData[(LEVEL1_HEIGHT - 2) * LEVEL1_WIDTH + c] = 1;
    }
    
    for (int i = 0; i < 4; ++i) {
        for (int c = 4 + i; c <= 6 + i; ++c)
            mLevelData[(LEVEL1_HEIGHT - 4 - i) * LEVEL1_WIDTH + c] = 3;
    }
    
    for (int c = 16; c <= 20; ++c)
        mLevelData[(LEVEL1_HEIGHT - 6) * LEVEL1_WIDTH + c] = 5;
    
    for (int c = 24; c <= 27; ++c)
        mLevelData[(LEVEL1_HEIGHT - 9) * LEVEL1_WIDTH + c] = 7;

    mGameState.bgm = LoadMusicStream("assets/game/looped_background_music.wav");
    SetMusicVolume(mGameState.bgm, 0.5f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/player_jump_sound.wav");

    mGameState.map = new Map(
        LEVEL1_WIDTH, LEVEL1_HEIGHT,
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
        {140.0f, 140.0f},
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

    std::map<Direction, std::vector<int>> skeletonAnimationAtlas = {
        {LEFT,  {0, 1, 2, 3, 4, 5}},  
        {RIGHT, {0, 1, 2, 3, 4, 5}}   
    };

    mSkeleton = new Entity(
        {mOrigin.x + 200.0f, mOrigin.y - 150.0f},
        {120.0f, 120.0f},
        "assets/game/skeleton_walk_right.png",
        NPC
    );
    mSkeleton->setLeftRightTextures(
        "assets/game/skeleton_walk_left.png",
        "assets/game/skeleton_walk_right.png",
        ATLAS,
        {1.0f, 6.0f} 
    );
    mSkeleton->setAnimationAtlas(skeletonAnimationAtlas);
    mSkeleton->setDirection(LEFT); 
    mSkeleton->setFrameSpeed(Entity::DEFAULT_FRAME_SPEED);
    mSkeleton->setAIType(WANDERER);
    mSkeleton->setAIState(WALKING);
    mSkeleton->setSpeed(100);
    mSkeleton->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mSkeleton->setColliderDimensions({15.0f, 15.0f});

    mGameState.camera = { 0 };
    mGameState.camera.target = mGameState.xochitl->getPosition();
    mGameState.camera.offset = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;
}

void Level1::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    // Update skeleton AI
    mSkeleton->update(
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
    if (mGameState.xochitl->checkCollisionWithAI(mSkeleton))
    {
        // Player loses a life 
        mGameState.nextSceneID = -1; 
    }

    // Check if player reached end of level
    if (mGameState.xochitl->getPosition().x > mOrigin.x + 800.0f)
    {
        mGameState.nextSceneID = 2; 
    }

    // Camera follows player
    Vector2 currentPlayerPosition = { mGameState.xochitl->getPosition().x, mOrigin.y };
    panCamera(&mGameState.camera, &currentPlayerPosition);
}

void Level1::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mSkeleton->render();
    mGameState.xochitl->render();
}

void Level1::shutdown()
{
    delete mGameState.xochitl;
    delete mGameState.map;
    delete mSkeleton;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
}

